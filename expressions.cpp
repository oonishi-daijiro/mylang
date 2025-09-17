#include "expressions.hpp"
#include "traits.hpp"

namespace Compiler {

// double expr
DoubleExpr::DoubleExpr(double val) : value{val} {};

llvm::Value *DoubleExpr::get() {
  return llvm::ConstantFP::get(type.getTypeInst(), value);
};

void DoubleExpr::resolveType() { type.resolve("double"); }

// integer expr
IntegerExpr::IntegerExpr(int32_t val) : value{val} {}

llvm::Value *IntegerExpr::get() {
  return llvm::ConstantInt::get(type.getTypeInst(), value);
};

void IntegerExpr::resolveType() { type.resolve("integer"); }

// boolean expr
BooleanExpr::BooleanExpr(bool b) : value{b} {};
llvm::Value *BooleanExpr::get() {
  return llvm::ConstantInt::get(type.getTypeInst(), value);
};
void BooleanExpr::resolveType() { type.resolve("boolean"); }

// variable
Variable::Variable(const std::string &name, Type *type)
    : name{name}, initialType{type} {}

Variable *Variable::DefineNewVariable(const std::string &name, Type &type) {
  if (varmap.count(name)) {
    throw SymbolError(std::format("variable {} is already defined", name));
  } else {
    auto ptr = new Variable{name, &type};
    varmap.emplace(name, ptr);
    return ptr;
  }
}

Variable::Variable(const std::string &name) : name{name} {
  if (!varmap.count(name)) {
    throw SymbolError(std::format("variable \"{}\" is not defined", name));
  } else {
    initialType = varmap.at(name)->initialType;
  }
}
void Variable::allocate() {
  if (pointer == nullptr) {
    pointer = builder->CreateAlloca(type.getTypeInst(), nullptr, name);
  }
}
llvm::Value *Variable::get() {
  if (pointer == nullptr) {
    pointer = varmap.at(name)->pointer;
  }
  return builder->CreateLoad(type.getTypeInst(), pointer);
};

void Variable::set(Value &val) {
  if (pointer == nullptr) {
    pointer = varmap.at(name)->pointer;
  }
  builder->CreateStore(val.get(), pointer);
}
llvm::Value *Variable::ptr() { return pointer; }

void Variable::resolveType() {
  if (initialType != nullptr) {
    type.resolve(initialType->name());
  } else {
    type.resolve(varmap.at(name)->type.name());
  }
}
const std::string &Variable::getname() { return name; }
} // namespace Compiler
