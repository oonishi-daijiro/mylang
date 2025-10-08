

#include <algorithm>
#include <llvm/IR/Value.h>
#include <sstream>
#include <stdexcept>

#include "errors.hpp"
#include "expressions.hpp"
#include "kind.hpp"
#include "operators.hpp"
#include "type.hpp"
#include "type_traits.hpp"
#include "utils.hpp"

namespace Compiler {
Type BooleanOperator::resultType() { return Type::GetType("boolean"); }

std::string AddOperator::kind() { return "+"; }
std::string SubOperator::kind() { return "-"; }
std::string MulOperator::kind() { return "*"; }
std::string DivOperator::kind() { return "/"; }
std::string EqOperator::kind() { return "=="; }
std::string NeqOperator::kind() { return "!="; }
std::string LtOperator::kind() { return "<"; }
std::string LeOperator::kind() { return "<="; }
std::string GtOperator::kind() { return ">"; }
std::string GeOperator::kind() { return ">="; }
std::string MinusOperator::kind() { return "(-)"; }
std::string IncrementOperator::kind() { return "()++"; }
std::string DecrementOperator::kind() { return "()--"; }
std::string kind() { return "[]"; }
std::string CallOperator::kind() { return "(...)"; };
// magma operator

MagmaOperator::MagmaOperator(Expression *lv, Expression *rv)
    : Operator{{lv, rv}}, lv{*lv}, rv{*rv} {}

void MagmaOperator::resolveType() {
  if (lv.type != rv.type) {
    throw TypeError(this->info, std::format("type mismatch: '{}' vs '{}'",
                                            lv.type.name(), rv.type.name()));
  } else {
    type = lv.type;
  }
};

// binary operator

BinaryOperator::BinaryOperator(Expression *lv, Expression *rv)
    : Operator{lv, rv}, lv{*lv}, rv{*rv} {}

void BinaryOperator::resolveType() {
  if (lv.type != rv.type) {
    throw TypeError(this->info, std::format("type mismatch: '{}' vs '{}'",
                                            lv.type.name(), rv.type.name()));
  } else {
    type = resultType();
  }
}

llvm::Value *AddOperator::get() {
  auto trait = type.trait()->expect<Field>();
  return trait->add(lv, rv);
}

llvm::Value *SubOperator::get() {
  auto trait = type.trait()->expect<Field>();
  return trait->sub(lv, rv);
}

llvm::Value *MulOperator::get() {
  auto trait = type.trait()->expect<Field>();
  return trait->mul(lv, rv);
}

llvm::Value *DivOperator::get() {
  auto trait = type.trait()->expect<Field>();
  return trait->div(lv, rv);
}

llvm::Value *MinusOperator::get() {
  auto trait = type.trait()->expect<Ordered>();
  return trait->minus(o);
}

llvm::Value *EqOperator::get() {
  auto trait = lv.type.trait()->expect<Boolean>();
  return trait->eq(lv, rv);
}

llvm::Value *NeqOperator::get() {
  auto trait = lv.type.trait()->expect<Boolean>();
  return trait->ne(lv, rv);
}

llvm::Value *LtOperator::get() {
  auto trait = lv.type.trait()->expect<Ordered>();
  return trait->lt(lv, rv);
}

llvm::Value *LeOperator::get() {
  auto trait = lv.type.trait()->expect<Ordered>();
  return trait->le(lv, rv);
}

llvm::Value *GtOperator::get() {
  auto trait = lv.type.trait()->expect<Ordered>();
  return trait->gt(lv, rv);
}

llvm::Value *GeOperator::get() {
  auto trait = lv.type.trait()->expect<Ordered>();
  return trait->ge(lv, rv);
}

llvm::Value *IncrementOperator::get() {
  auto trait = sub.type.trait()->expect<Field>();
  auto &unit = trait->unit();
  auto prev = sub.get();
  builder->CreateStore(trait->add(sub, unit), mutSub->ptr());
  return prev;
}

llvm::Value *DecrementOperator::get() {
  auto trait = sub.type.trait()->expect<Field>();
  auto &unit = trait->unit();
  auto prev = sub.get();
  builder->CreateStore(trait->sub(sub, unit), mutSub->ptr());
  return prev;
}

// indexing operator

IndexingOperator::IndexingOperator(Expression *arraylike, Expression *index)
    : Operator{{arraylike, index}}, index{*index}, arraylike{*arraylike} {}

void IndexingOperator::set(Value &val) {
  // if (arraylike.type.kind()->isa<StringKind>()) {
  //   throw TypeError(info, std::format("cannot set value to string literal"));
  // }
  auto ptr = this->ptr();
  builder->CreateStore(val.get(), ptr);
};

llvm::Value *IndexingOperator::get() {
  auto ptr = this->ptr();
  auto elmTy = arraylike.type.kind()->cast<ArrayKind>()->element();
  auto elm = builder->CreateLoad(elmTy.getTypeInst(), ptr);
  return elm;
}

llvm::Value *IndexingOperator::ptr() {
  auto indexable = arraylike.type.trait()->expect<Indexable>();
  if (index.isa<ConstantEval<int32_t>>() &&
      arraylike.type.kind()->isa<ArrayKind>()) {
    auto idx = index.cast<ConstantEval<int32_t>>()->val();
    auto arraysize = arraylike.type.kind()->cast<ArrayKind>()->size();

    if (idx >= arraysize) {
      throw RangeError(
          this->info,
          std::format("index out of range. size is {} but index is {}",
                      arraysize, idx));
    }
  }
  auto ptr = indexable->at(arraylike, index);
  return ptr;
}

void IndexingOperator::resolveType() {
  auto indexable = false;
  indexable |= arraylike.type.kind()->isa<ArrayKind>();
  indexable |= arraylike.type == "string";

  if (!indexable) {
    throw TypeError(this->info, std::format("cannot index access to type : {}",
                                            arraylike.type.name()));
  } else {
    if (arraylike.type == "string") {
      type = "char";
    } else {
      type = arraylike.type.kind()->cast<ArrayKind>()->element();
    }
  }
}

// CallOperator

CallOperator::CallOperator(Expression *callable,
                           std::vector<Expression *> &&args)
    : callable{*callable}, Operator{callable} {
  for (auto &&a : args) {
    appendChild(a);
    arguments.emplace_back(a);
  }
}

void CallOperator::resolveType() {
  if (!callable.type.kind()->isa<FunctionKind>()) {
    throw TypeError(info,
                    std::format("{} is not function", callable.type.name()));
  } else {
    auto func = callable.type.kind()->cast<FunctionKind>();
    std::vector<const Type *> passedArgTy{};
    for (auto &&v : arguments) {
      passedArgTy.emplace_back(&v->type);
    }

    if (passedArgTy.size() != func->signature().arguments().size()) {
      throw TypeError(
          info, std::format(
                    "invalid function arguments. requested {} arguments but {}",
                    func->signature().arguments().size(), passedArgTy.size()));
    } else {
      if (passedArgTy == func->signature().argType()) {
        type = func->signature().returnType().value();
      } else {
        std::stringstream sigArgStr;
        std::stringstream passedArgStr;

        auto sigArg = func->signature().argType();
        for (int i = 0; i < sigArg.size(); i++) {
          sigArgStr << sigArg[i]->name() << (i < sigArg.size() - 1 ? "," : "");
          passedArgStr << passedArgTy[i]->name()
                       << (i < sigArg.size() - 1 ? "," : "");
        }
        throw TypeError(info, std::format("argument type mismatching {} vs {}",
                                          sigArgStr.str(), passedArgStr.str()));
      }
    }
  }
};

llvm::Value *CallOperator::get() {
  auto fn = callable.type.trait()->expect<Callable>();
  auto ret = fn->call(callable, arguments);
  return ret;
}

}; // namespace Compiler
