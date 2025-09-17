#pragma once

#include "ast.hpp"
#include "errors.hpp"
#include "traits.hpp"

#include <cstdint>
#include <format>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <set>
#include <string>

namespace Compiler {

class Expression : public Statement, public Value {
public:
  using Statement::Statement;
  virtual ~Expression() = default;

  virtual void gen() override final { get(); };
  virtual void init() override final { resolveType(); };
  virtual llvm::Value *get() override = 0;
  virtual void resolveType() = 0;

  virtual std::string to_string() override { return "[***expression***]"; };
};

class DoubleExpr : public Expression {
  double value;

public:
  DoubleExpr(double val);

  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;
};

class IntegerExpr : public Expression {
  int32_t value;

public:
  IntegerExpr(int32_t val);

  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;
};

class BooleanExpr final : public Expression {
  bool value;

public:
  BooleanExpr(bool b);
  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;
};

class StringExpr final : public Expression {
  std::string value;

public:
  StringExpr(const std::string &str) : value{str} {}
  virtual llvm::Value *get() override {};
  virtual std::string to_string() override {};
  virtual void resolveType() override {};
};

class Substance : public Expression {
public:
  virtual llvm::Value *get() = 0;
  virtual void set(Value &) = 0;
  virtual llvm::Value *ptr() = 0;
};

class Variable final : public Substance {
  Type *initialType{nullptr};
  llvm::AllocaInst *pointer{nullptr};
  std::string name;
  static inline std::map<std::string, Variable *> varmap{};

  Variable(const std::string &name, Type *type);

public:
  static Variable *DefineNewVariable(const std::string &name, Type &type);
  Variable(const std::string &name);
  virtual std::string to_string() override;
  void allocate();
  virtual llvm::Value *get() override;
  virtual void set(Value &val) override;
  virtual llvm::Value *ptr() override;
  virtual void resolveType() override;
  const std::string &getname();
};

} // namespace Compiler
