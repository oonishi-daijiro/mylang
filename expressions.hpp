#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <llvm/IR/DerivedTypes.h>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "errors.hpp"
#include "kind.hpp"
#include "scope.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "type_traits.hpp"
#include "utils.hpp"
#include "value.hpp"

namespace Compiler {

class Expression : public Statement, public Value {
public:
  using Statement::Statement;
  virtual ~Expression() = default;

  virtual void gen() override final {
    try {
      get();
    } catch (std::runtime_error &e) {
      throw CodeGenError(info, e.what());
    }
  };
  virtual llvm::Value *get() override = 0;

  virtual std::string to_string() override { return "[***expression***]"; };
};

class Literal {
public:
  virtual ~Literal() = default;
  virtual std::string value_str() = 0;
};

template <typename T> class ConstantEval {
private:
  T value;

public:
  virtual ~ConstantEval() = default;
  ConstantEval(T val) : value{val} {};
  virtual T &val() final { return value; }
};

class DoubleExpr : public Expression,
                   public ConstantEval<double>,
                   public Literal {
  double value;

public:
  DoubleExpr(double val);

  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;
  virtual std::string value_str() override { return std::to_string(value); }
};

class IntegerExpr : public Expression,
                    public ConstantEval<int32_t>,
                    public Literal {
  int32_t value;

public:
  IntegerExpr(int32_t val);

  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;

  virtual std::string value_str() override { return std::to_string(value); }
};

class BooleanExpr final : public Expression,
                          public ConstantEval<bool>,
                          public Literal {
  bool value;

public:
  BooleanExpr(bool b);
  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;
  virtual std::string value_str() override { return std::to_string(value); }
};

class ArrayExpr final : public Expression {
  std::vector<Value *> elements{};
  std::stringstream valstr{};

public:
  ArrayExpr(std::vector<Expression *> &&initExpr);
  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;
};

class StringExpr final : public Expression,
                         public ConstantEval<std::string>,
                         public Literal {
  std::string value;

public:
  StringExpr(const std::string &str);
  virtual llvm::Value *get() override;
  virtual std::string to_string() override;
  virtual void resolveType() override;
  virtual std::string value_str() override;
};

class Substance : public virtual Expression {
public:
  virtual ~Substance() = default;
  virtual void set(Value &) = 0;
  virtual llvm::Value *ptr() = 0;
};

class LocalVariable final : public Substance, public Symbol {
  Type *initialType;
  llvm::AllocaInst *pointer{nullptr};
  std::string name;
  const bool isReference;

public:
  LocalVariable(const std::string &name, Type *type);
  LocalVariable(const std::string &name);
  void allocate();

  virtual std::string to_string() override;
  virtual llvm::Value *get() override;
  virtual void set(Value &val) override;
  virtual llvm::Value *ptr() override;
  virtual const std::string kind() const override { return "LocalVariable"; };

  virtual void resolveType() override;
  virtual void resolveSymbol() override;
  virtual void init() override;

  const std::string &getname();
};

} // namespace Compiler
