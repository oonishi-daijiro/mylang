

#include <llvm/IR/Value.h>

#include "expressions.hpp"
#include "operators.hpp"
#include "type_traits.hpp"

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
std::string IncrementOperator::kind() { return "(...)++"; }
std::string DecrementOperator::kind() { return "(...)--"; }

MagmaOperator::MagmaOperator(Expression *lv, Expression *rv)
    : Operator{{lv, rv}}, lv{*lv}, rv{*rv} {}

void MagmaOperator::resolveType() {
  if (lv.type != rv.type) {
    throw TypeError(this->info, std::format("type mismatch: '{}' vs '{}'",
                                            lv.type.name(), rv.type.name()));
  } else {
    this->type = lv.type;
  }
};

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
  auto trait = type.trait()->except<Field>();
  return trait->add(lv, rv);
}

llvm::Value *SubOperator::get() {
  auto trait = type.trait()->except<Field>();
  return trait->sub(lv, rv);
}

llvm::Value *MulOperator::get() {
  auto trait = type.trait()->except<Field>();
  return trait->mul(lv, rv);
}

llvm::Value *DivOperator::get() {
  auto trait = type.trait()->except<Field>();
  return trait->div(lv, rv);
}

llvm::Value *MinusOperator::get() {
  auto trait = type.trait()->except<Ordered>();
  return trait->minus(o);
}

llvm::Value *EqOperator::get() {
  auto trait = lv.type.trait()->except<Boolean>();
  return trait->eq(lv, rv);
}

llvm::Value *NeqOperator::get() {
  auto trait = lv.type.trait()->except<Boolean>();
  return trait->ne(lv, rv);
}

llvm::Value *LtOperator::get() {
  auto trait = lv.type.trait()->except<Ordered>();
  return trait->lt(lv, rv);
}

llvm::Value *LeOperator::get() {
  auto trait = lv.type.trait()->except<Ordered>();
  return trait->le(lv, rv);
}

llvm::Value *GtOperator::get() {
  auto trait = lv.type.trait()->except<Ordered>();
  return trait->gt(lv, rv);
}

llvm::Value *GeOperator::get() {
  auto trait = lv.type.trait()->except<Ordered>();
  return trait->ge(lv, rv);
}

llvm::Value *IncrementOperator::get() {
  auto trait = o->type.trait()->except<Field>();
  auto &unit = trait->unit();
  auto prev = o->get();
  builder->CreateStore(trait->add(*o, unit), o->ptr());
  return prev;
}

llvm::Value *DecrementOperator::get() {
  auto trait = o->type.trait()->except<Field>();
  auto &unit = trait->unit();
  auto prev = o->get();
  builder->CreateStore(trait->sub(*o, unit), o->ptr());
  return prev;
}

// indexing operator

IndexingOperator::IndexingOperator(Expression *arraylike, Expression *index)
    : Operator{{arraylike, index}}, index{*index}, arraylike{*arraylike} {}

std::string kind() { return "[]"; }

void IndexingOperator::set(Value &val) {
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
  auto indexable = arraylike.type.trait()->except<Indexable>();
  if (index.isa<ConstantEval<int32_t>>()) {
    auto idx = index.cast<ConstantEval<int32_t>>()->val();
    auto arraysize = arraylike.type.kind()->cast<ArrayKind>()->size();
    if (idx >= arraysize) {
      throw RangeError(
          this->info,
          std::format("index out of range. array size is {} but index is {}",
                      arraysize, idx));
    }
  }
  auto ptr = indexable->at(arraylike, index);
  return ptr;
}
void IndexingOperator::resolveType() {

  if (!arraylike.type.kind()->isa<ArrayKind>()) {
    throw TypeError(this->info,
                    std::format("operand type {} must be array kind but {}",
                                arraylike.type.name(),
                                arraylike.type.kind()->name()));
  } else {
    type = arraylike.type.kind()->cast<ArrayKind>()->element();
  }
}

}; // namespace Compiler
