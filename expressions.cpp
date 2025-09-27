#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>
#include <vector>

#include "errors.hpp"
#include "expressions.hpp"
#include "function.hpp"
#include "kind.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "utils.hpp"
#include "value.hpp"

namespace Compiler {

// double expr
DoubleExpr::DoubleExpr(double val) : ConstantEval{val}, value{val} {};

llvm::Value *DoubleExpr::get() { return llvm::ConstantFP::get(type, value); };

void DoubleExpr::resolveType() { type = "double"; }

// integer expr
IntegerExpr::IntegerExpr(int32_t val) : ConstantEval{val}, value{val} {}

llvm::Value *IntegerExpr::get() { return llvm::ConstantInt::get(type, value); };

void IntegerExpr::resolveType() { type = "integer"; }

// boolean expr
BooleanExpr::BooleanExpr(bool b) : ConstantEval{b}, value{b} {};
llvm::Value *BooleanExpr::get() { return llvm::ConstantInt::get(type, value); };
void BooleanExpr::resolveType() { type = "boolean"; }

// array expr

ArrayExpr::ArrayExpr(std::vector<Expression *> &&initExprs)
    : elements(initExprs.size()) {
  for (size_t i = 0; i < initExprs.size(); i++) {
    appendChild(initExprs[i]);
    elements[i] = initExprs[i];
    if (initExprs[i]->isa<Literal>()) {
      auto str = initExprs[i]->cast<Literal>()->value_str();
      valstr << str;
    } else {
      valstr << initExprs[i]->type.name();
    }
    valstr << ((i == initExprs.size() - 1) ? "" : " , ");
  }
}

// array expr
llvm::Value *ArrayExpr::get() {
  auto arraysize = builder->getInt64(elements.size());
  auto elementTy = type.kind()->cast<ArrayKind>()->element();
  auto arrayTy = llvm::ArrayType::get(elementTy, elements.size());
  auto head = builder->CreateAlloca(arrayTy, nullptr);
  auto zero = builder->getInt64(0);

  for (size_t i = 0; i < elements.size(); i++) {
    auto index = builder->getInt64(i);
    auto p = builder->CreateGEP(arrayTy, head, {zero, index});
    builder->CreateStore(elements[i]->get(), p);
  }

  return head;
};

void ArrayExpr::resolveType() {
  if (elements.size() > 0) {
    bool isAllSameTy = true;
    for (int i = 0; i < elements.size() - 1; i++) {
      isAllSameTy &= elements[i]->type == elements[i + 1]->type;
    }

    if (!isAllSameTy) {
      throw TypeError(this->info, "array element must all same type");
    }

    type = ArrayKind::Apply(elements[0]->type, elements.size());
  } else {
    throw TypeError(this->info,
                    "array expression must be have 1 or more elements");
  }
};

// string expr

StringExpr::StringExpr(const std::string &str)
    : ConstantEval<std::string>{str}, value{str} {}

llvm::Value *StringExpr::get() {
  auto arraysize = builder->getInt64(value.size() + 1);
  auto elementTy = Type::GetType("char");
  auto arrayTy = llvm::ArrayType::get(elementTy, value.size() + 1);

  std::vector<llvm::Constant *> initVec{};

  for (size_t i = 0; i < value.size(); i++) {
    initVec.emplace_back(llvm::ConstantInt::get(elementTy, value[i]));
  }
  initVec.emplace_back(llvm::ConstantInt::get(elementTy, '\0'));

  auto constantArray = llvm::ConstantArray::get(arrayTy, initVec);

  auto strLiteralPointer = new llvm::GlobalVariable(
      *llvmModule, arrayTy, true, llvm::GlobalValue::ExternalLinkage,
      constantArray);
  return strLiteralPointer;
};

void StringExpr::resolveType() { type = StringKind::Apply(value.size() + 1); };

std::string StringExpr::value_str() { return std::format("\"{}\"", value); };

// LocalVar

LocalVar::LocalVar(const std::string &name, Type &ty)
    : name{name}, initialType{ty}, Variable{name} {};

void LocalVar::allocate() { pointer = builder->CreateAlloca(type, nullptr); };

llvm::Value *LocalVar::get() { return builder->CreateLoad(type, pointer); };

void LocalVar::set(Value &val) { builder->CreateStore(val.get(), pointer); };

llvm::Value *LocalVar::ptr() { return pointer; };

const std::string LocalVar::kind() const { return "local variable"; };

void LocalVar::resolveType() { type = initialType; };

void LocalVar::resolveSymbol() {
  if (currentScope().existsOnSameScope(name)) {
    auto s = currentScope().find(name);
    throw SymbolError(
        info, std::format("symbol {} is already defined as", name, s->name()));
  } else {
    registerToCurrentScope();
  }
};

const std::string &LocalVar::getname() { return name; };

// SymbolReferenceExpr

void SymbolReferenceExpr::resolveSymbol() {
  if (currentScope().available(name)) {
    sym = currentScope().find(name);
    if (sym->isa<Variable, Function>()) {
      this->referValue = sym->cast<Value>();

    } else {
      throw SymbolError(info,
                        std::format("symbol {} does not refer to value", name));
    }
  } else {
    throw SymbolError(info, std::format("symbol {} is not defined", name));
  }
}

llvm::Value *SymbolReferenceExpr::ptr() {
  auto maybeSub = referValue->getSubstance();

  auto visitor = util::visitors{
      [](Substance<Mutable> *mut) -> llvm::Value * { return mut->ptr(); },
      [](Substance<Immutable> *imut) -> llvm::Value * { return imut->ptr(); },
      [](Value *val) -> llvm::Value * { return nullptr; }};

  auto ptr = std::visit(visitor, maybeSub);
  if (ptr == nullptr) {
    throw TypeError(info,
                    std::format("cannot get pointer of non substance value"));
  }
  return ptr;
}

void SymbolReferenceExpr::set(Value &val) {
  using setFunc = std::function<void(Value &)>;

  auto maybeSub = referValue->getSubstance();
  auto invalOpFunc = [&](Value &, const std::string &msg) {
    throw TypeError(info, msg);
  };

  auto visitor = util::visitors{
      [&](Substance<Mutable> *mut) -> setFunc {
        return {[mut](Value &v) { mut->set(v); }};
      },
      [&](Substance<Immutable> *imut) -> setFunc {
        return {[&](Value &v) {
          invalOpFunc(v, "cannot set value to immutable object");
        }};
      },
      [&](Value *v) -> setFunc {
        return {[&](Value &v) {
          invalOpFunc(v, "cannot set value to object that has non-substance");
        }};
      }};
  auto setter = std::visit(visitor, maybeSub);
  setter(val);
}

} // namespace Compiler
