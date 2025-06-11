#include "expressions.hpp"
#include "ast.hpp"
#include "token.hpp"
#include "traits.hpp"
#include <llvm/IR/Constant.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>

namespace Compiler {

llvm::Value *Double::add(Value &rv) {
  return builder->CreateFAdd(get(), rv.get());
};

llvm::Value *Double::sub(Value &rv) {
  return builder->CreateFSub(get(), rv.get());
};

llvm::Value *Double::mul(Value &rv) {
  return builder->CreateFMul(get(), rv.get());
};

llvm::Value *Double::div(Value &rv) {
  return builder->CreateFDiv(get(), rv.get());
};

llvm::Value *Double::minus() {
  auto zero = llvm::ConstantFP::get(resolve(), 0);
  return builder->CreateFSub(zero, get());
};

llvm::Value *Double::plus() { return get(); };

llvm::Value *Double::fcmp(llvm::CmpInst::Predicate predicate, Value &rv) {
  return builder->CreateFCmp(predicate, get(), rv.get());
}

llvm::Value *Double::eq(Value &rv) {
  return fcmp(llvm::CmpInst::FCMP_OEQ, rv);
};

llvm::Value *Double::ne(Value &rv) {
  return fcmp(llvm::CmpInst::FCMP_ONE, rv);
};

llvm::Value *Double::lt(Value &rv) {
  return fcmp(llvm::CmpInst::FCMP_OLT, rv);
};

llvm::Value *Double::le(Value &rv) {
  return fcmp(llvm::CmpInst::FCMP_OLE, rv);
};

llvm::Value *Double::gt(Value &rv) {
  return fcmp(llvm::CmpInst::FCMP_OGT, rv);
};

llvm::Value *Double::ge(Value &rv) {
  return fcmp(llvm::CmpInst::FCMP_OGE, rv);
};

llvm::Value *Integer::icmp(llvm::ICmpInst::Predicate predicate, Value &rv) {
  return builder->CreateICmp(predicate, get(), rv.get());
};

llvm::Value *Integer::add(Value &rv) {
  return builder->CreateAdd(get(), rv.get());
};
;

llvm::Value *Integer::sub(Value &rv) {
  return builder->CreateSub(get(), rv.get());
};

llvm::Value *Integer::mul(Value &rv) {
  return builder->CreateMul(get(), rv.get());
};

llvm::Value *Integer::div(Value &rv) {
  return builder->CreateSDiv(get(), rv.get());
};

llvm::Value *Integer::minus() {
  auto zero = llvm::ConstantInt::get(resolve(), 0);
  return builder->CreateSub(zero, get());
};

llvm::Value *Integer::plus() { return get(); };

llvm::Value *Integer::eq(Value &rv) {
  return icmp(llvm::ICmpInst::ICMP_EQ, rv);
};

llvm::Value *Integer::ne(Value &rv) {
  return icmp(llvm::ICmpInst::ICMP_NE, rv);
};

llvm::Value *Integer::lt(Value &rv) {
  return icmp(llvm::ICmpInst::ICMP_SLT, rv);
};

llvm::Value *Integer::le(Value &rv) {
  return icmp(llvm::ICmpInst::ICMP_SLE, rv);
};

llvm::Value *Integer::gt(Value &rv) {
  return icmp(llvm::ICmpInst::ICMP_SGT, rv);
};

llvm::Value *Integer::ge(Value &rv) {
  return icmp(llvm::ICmpInst::ICMP_SGE, rv);
};

} // namespace Compiler
