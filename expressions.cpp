#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <vector>

#include "control_statements.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "kind.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "utils.hpp"

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

// variable
LocalVariable::LocalVariable(const std::string &name, Type *type)
    : name{name}, initialType{type}, Symbol{name}, isReference{false} {}

LocalVariable::LocalVariable(const std::string &name)
    : name{name}, Symbol{name}, isReference{true} {}

void LocalVariable::allocate() {
  if (!isReference) {
    pointer = builder->CreateAlloca(type, nullptr, name);
  } else {
    pointer = currentScope().find(name)->expect<LocalVariable>()->pointer;
  }
}

llvm::Value *LocalVariable::get() {
  if (isReference && pointer == nullptr) {
    pointer = currentScope().find(name)->expect<LocalVariable>()->pointer;
  }
  return builder->CreateLoad(type, pointer);
};

void LocalVariable::set(Value &val) {
  if (isReference && pointer == nullptr) {
    pointer = currentScope().find(name)->expect<LocalVariable>()->pointer;
  }
  builder->CreateStore(val.get(), pointer);
}

llvm::Value *LocalVariable::ptr() {
  if (isReference && pointer == nullptr) {
    pointer = currentScope().find(name)->expect<LocalVariable>()->pointer;
  }
  return pointer;
}

void LocalVariable::resolveType() {
  if (initialType != nullptr) {
    type = *initialType;
  } else {
    throw TypeError(info, std::format("unable to resolve type"));
  }
}

const std::string &LocalVariable::getname() { return name; }
void LocalVariable::init() {}

void LocalVariable::resolveSymbol() {
  if (isReference) {
    if (currentScope().available(name)) {
      auto sym = currentScope().find(name);
      if (sym->isa<LocalVariable>()) {
        auto lclVar = sym->cast<LocalVariable>();
        initialType = lclVar->initialType;
      } else {
        throw SymbolError(info,
                          std::format("symbol {} is defined but defined as {}",
                                      name, sym->kind()));
      }
    } else {
      throw SymbolError(
          info, std::format("local variable \"{}\" is not defined", name));
    }
  } else {
    if (currentScope().existsOnSameScope(name)) {
      throw SymbolError(
          info,
          std::format("symbol {} is already defined on the same scope as a {}",
                      name, currentScope().find(name)->kind()));
    } else {
      registerToCurrentScope();
    }
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

} // namespace Compiler
