#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <vector>

#include "errors.hpp"
#include "expressions.hpp"
#include "kind.hpp"
#include "symbol.hpp"
#include "type.hpp"

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

// VariableReference

VariableReference::VariableReference(const std::string &name)
    : Variable{name}, name{name} {}

void VariableReference::resolveSymbol() {
  if (currentScope().available(name)) {
    auto symbol = currentScope().find(name);
    if (symbol->isa<Variable>()) {
      var = symbol->cast<Variable>();
    } else {
      throw SymbolError(info, std::format("symbol \"{}\" is defined as \"{}\"",
                                          name, symbol->kind()));
    }
  } else {
    throw SymbolError(info, std::format("symbol \"{}\" is not defined", name));
  }
}

const std::string VariableReference::kind() const { return var->kind(); }
llvm::Value *VariableReference::get() { return var->get(); };
std::string VariableReference::to_string() { return var->to_string(); }
void VariableReference::set(Value &val) { var->set(val); };
llvm::Value *VariableReference::ptr() { return var->ptr(); };
void VariableReference::resolveType() { type = var->type; }

// FunctionReference

const std::string FunctionReference::kind() const { return func->kind(); };
llvm::Value *FunctionReference::get() { return func->funcPtr(); };
std::string FunctionReference::to_string() { return func->to_string(); };
void FunctionReference::resolveSymbol() {
  if (currentScope().available(name)) {
    auto symbol = currentScope().find(name);
    if (symbol->isa<Function>()) {
      func = symbol->cast<Function>();
    } else {
      throw SymbolError(info,
                        std::format("symbol \"{}\" is not function \"{}\"",
                                    name, symbol->kind()));
    }
  } else {
    throw SymbolError(info, std::format("symbol \"{}\" is not defined", name));
  }
};

void FunctionReference::resolveType() { type = func->type(); };

} // namespace Compiler
