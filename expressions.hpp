#pragma once

#include "ast.hpp"
#include "traits.hpp"

#include <cstdint>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace Compiler {

class Numeric : virtual public Expression, virtual public Ordered {};

class Double : virtual public Numeric {
  double value;
  llvm::Value *fcmp(llvm::FCmpInst::Predicate, Value &);

public:
  Double(double val) : Type{"double"}, value{val} {};

  virtual llvm::Value *get() override {
    return llvm::ConstantFP::get(resolve(), value);
  };

  virtual std::string to_string() override;

  virtual llvm::Value *add(Value &rv) override;
  virtual llvm::Value *sub(Value &rv) override;
  virtual llvm::Value *mul(Value &rv) override;
  virtual llvm::Value *div(Value &rv) override;

  virtual llvm::Value *minus() override;
  virtual llvm::Value *plus() override;

  virtual llvm::Value *eq(Value &rv) override;
  virtual llvm::Value *ne(Value &rv) override;
  virtual llvm::Value *lt(Value &rv) override;
  virtual llvm::Value *le(Value &rv) override;
  virtual llvm::Value *gt(Value &rv) override;
  virtual llvm::Value *ge(Value &rv) override;
};

class Integer : public Numeric {
  int32_t value;
  llvm::Value *icmp(llvm::ICmpInst::Predicate, Value &);

public:
  Integer(int32_t val) : Type{"integer"}, value{val} {}

  virtual llvm::Value *get() override {
    std::cout << "get value" << std::endl;
    return llvm::ConstantInt::get(resolve(), value);
  };

  virtual std::string to_string() override;

  virtual llvm::Value *add(Value &rv) override;
  virtual llvm::Value *sub(Value &rv) override;
  virtual llvm::Value *mul(Value &rv) override;
  virtual llvm::Value *div(Value &rv) override;

  virtual llvm::Value *minus() override;
  virtual llvm::Value *plus() override;

  virtual llvm::Value *eq(Value &rv) override;
  virtual llvm::Value *ne(Value &rv) override;
  virtual llvm::Value *lt(Value &rv) override;
  virtual llvm::Value *le(Value &rv) override;
  virtual llvm::Value *gt(Value &rv) override;
  virtual llvm::Value *ge(Value &rv) override;
};

class Substance : public Expression {
public:
  virtual llvm::Value *get() = 0;
  virtual void set(llvm::Value *) = 0;

  virtual llvm::Value *ptr() = 0;

  virtual llvm::Value *eval() final { return get(); };
};

class Variable : public Substance {};

} // namespace Compiler
