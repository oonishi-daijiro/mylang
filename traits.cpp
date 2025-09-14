#include "traits.hpp"
#include <llvm/IR/Value.h>
namespace Compiler {
llvm::Value *DoubleTyTrait::add(Value &lv, Value &rv) {
  return builder->CreateFAdd(lv.get(), rv.get());
};

llvm::Value *DoubleTyTrait::sub(Value &lv, Value &rv) {
  return builder->CreateFSub(lv.get(), rv.get());
};
llvm::Value *DoubleTyTrait::mul(Value &lv, Value &rv) {
  return builder->CreateFMul(lv.get(), rv.get());
}
llvm::Value *DoubleTyTrait::div(Value &lv, Value &rv) {
  return builder->CreateFDiv(lv.get(), rv.get());
}
llvm::Value *DoubleTyTrait::minus(Value &v) {
  return builder->CreateFNeg(v.get());
}
llvm::Value *DoubleTyTrait::plus(Value &v) { return v.get(); }

llvm::Value *DoubleTyTrait::eq(Value &lv, Value &rv) {
  // Floating point equality comparison
  return builder->CreateFCmpOEQ(lv.get(), rv.get());
}
llvm::Value *DoubleTyTrait::ne(Value &lv, Value &rv) {
  return builder->CreateFCmpONE(lv.get(), rv.get());
}
llvm::Value *DoubleTyTrait::lt(Value &lv, Value &rv) {
  return builder->CreateFCmpOLT(lv.get(), rv.get());
}
llvm::Value *DoubleTyTrait::le(Value &lv, Value &rv) {
  return builder->CreateFCmpOLE(lv.get(), rv.get());
}
llvm::Value *DoubleTyTrait::gt(Value &lv, Value &rv) {
  return builder->CreateFCmpOGT(lv.get(), rv.get());
}
llvm::Value *DoubleTyTrait::ge(Value &lv, Value &rv) {
  return builder->CreateFCmpOGE(lv.get(), rv.get());
}
llvm::Value *IntegerTyTrait::add(Value &lv, Value &rv) {
  return builder->CreateAdd(lv.get(), rv.get());
}
llvm::Value *IntegerTyTrait::sub(Value &lv, Value &rv) {
  return builder->CreateSub(lv.get(), rv.get());
}
llvm::Value *IntegerTyTrait::mul(Value &lv, Value &rv) {
  return builder->CreateMul(lv.get(), rv.get());
}
llvm::Value *IntegerTyTrait::div(Value &lv, Value &rv) {
  return builder->CreateSDiv(lv.get(), rv.get());
}

llvm::Value *IntegerTyTrait::minus(Value &v) {
  return builder->CreateNeg(v.get());
}

llvm::Value *IntegerTyTrait::plus(Value &v) { return v.get(); }

llvm::Value *IntegerTyTrait::eq(Value &lv, Value &rv) {
  return builder->CreateICmpEQ(lv.get(), rv.get());
}

llvm::Value *IntegerTyTrait::ne(Value &lv, Value &rv) {
  return builder->CreateICmpNE(lv.get(), rv.get());
}
llvm::Value *IntegerTyTrait::lt(Value &lv, Value &rv) {
  return builder->CreateICmpSLT(lv.get(), rv.get());
}
llvm::Value *IntegerTyTrait::le(Value &lv, Value &rv) {
  return builder->CreateICmpSLE(lv.get(), rv.get());
}
llvm::Value *IntegerTyTrait::gt(Value &lv, Value &rv) {
  return builder->CreateICmpSGT(lv.get(), rv.get());
}
llvm::Value *IntegerTyTrait::ge(Value &lv, Value &rv) {
  return builder->CreateICmpSGE(lv.get(), rv.get());
}

llvm::Value *BooleanTyTrait::eq(Value &lv, Value &rv) {
  return builder->CreateICmpEQ(lv.get(), rv.get());
}

llvm::Value *BooleanTyTrait::ne(Value &lv, Value &rv) {
  return builder->CreateICmpNE(lv.get(), rv.get());
}
}; // namespace Compiler
