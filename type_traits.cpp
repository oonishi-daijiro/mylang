#include "type_traits.hpp"
#include "errors.hpp"
#include "kind.hpp"
#include "type.hpp"
#include "utils.hpp"
#include "value.hpp"
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>
#include <stdexcept>

namespace Compiler {

Value &DoubleTyTrait::unit() {
  if (unitVal != nullptr) {
    unitVal = new Compiler::ConstantValue(
        llvm::ConstantFP::get(builder->getDoubleTy(), 1.0), "double");
  }
  return *unitVal;
}

DoubleTyTrait::~DoubleTyTrait() {
  if (unitVal != nullptr) {
    delete unitVal;
  }
}

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

Value &IntegerTyTrait::unit() {
  if (unitVal == nullptr) {
    auto llUnitval = llvm::ConstantInt::get(builder->getInt32Ty(), 1);
    unitVal = new ConstantValue(llUnitval, "integer");
  }
  return *unitVal;
}

IntegerTyTrait::~IntegerTyTrait() {
  if (unitVal != nullptr) {
    delete unitVal;
  }
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

llvm::Value *ArrayTyTrait::at(Value &arraylike, Value &idx) {
  auto arraykind = arraylike.type.kind()->cast<ArrayKind>();
  auto ptr = arraylike.get();
  auto arrayTy = llvm::ArrayType::get(arraykind->element().getTypeInst(),
                                      arraykind->size());
  auto zero = builder->getInt64(0);
  auto elmptr = builder->CreateGEP(arrayTy, ptr, {zero, idx.get()});
  return elmptr;
};

// string type trait

llvm::Value *StringTyTrait::at(Value &arraylike, Value &idx) {
  auto arraykind = arraylike.type.kind()->cast<ArrayKind>();
  auto ptr = arraylike.get();
  auto charTy = Type::GetType("char").getTypeInst();
  auto arrayTy = llvm::ArrayType::get(charTy, arraykind->size());
  auto zero = builder->getInt64(0);
  auto elmptr = builder->CreateGEP(arrayTy, ptr, {zero, idx.get()});
  return elmptr;
}

// character type trait

llvm::Value *CharacterTyTrait::eq(Value &lv, Value &rv) {
  return builder->CreateICmpEQ(lv.get(), rv.get());
}

llvm::Value *CharacterTyTrait::ne(Value &lv, Value &rv) {
  return builder->CreateICmpNE(lv.get(), rv.get());
}

} // namespace Compiler
