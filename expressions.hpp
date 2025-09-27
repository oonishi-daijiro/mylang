#pragma once

#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "errors.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "utils.hpp"
#include "value.hpp"

namespace Compiler {

class Expression : public Statement, public Value, public TypeSemantic {
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

  virtual std::string to_string() override {
    return "[***unknown_expression***]";
  };
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
  virtual ~DoubleExpr() = default;

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
  virtual ~IntegerExpr() = default;

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
  virtual ~BooleanExpr() = default;

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
  virtual ~ArrayExpr() = default;

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
  virtual ~StringExpr() = default;

  virtual llvm::Value *get() override;
  virtual std::string to_string() override;
  virtual void resolveType() override;
  virtual std::string value_str() override;
};

class Variable : public Substance<Mutable>,
                 public Symbol,
                 public SymbolSemantic,
                 public Expression {
public:
  Variable(const std::string &name) : Symbol{name} {}
  virtual ~Variable() = default;
};

class LocalVar : public Variable {
  Type &initialType;
  Type resolvedType;
  const std::string name;
  llvm::AllocaInst *pointer{nullptr};

public:
  LocalVar(const std::string &name, Type &ty);
  virtual ~LocalVar() = default;

  void allocate();
  virtual std::string to_string() override;
  virtual llvm::Value *get() override;
  virtual void set(Value &val) override;
  virtual llvm::Value *ptr() override;
  virtual const std::string kind() const override;
  virtual void resolveType() override;
  virtual void resolveSymbol() override;
  const std::string &getname();
};

class SymbolReferenceExpr : public Expression,
                            public SymbolSemantic,
                            public Symbol,
                            public Substance<Mutable> {
  const std::string name;
  Value *referValue{nullptr};
  Symbol *sym{nullptr};

public:
  SymbolReferenceExpr(const std::string &name) : Symbol{name}, name{name} {}

  virtual std::string to_string() override;

  virtual llvm::Value *ptr() override;

  virtual void set(Value &val) override;

  virtual llvm::Value *get() override { return referValue->get(); };

  virtual const std::string kind() const override { return sym->kind(); };
  virtual void resolveType() override { type = referValue->type; };
  virtual void resolveSymbol() override;
};

} // namespace Compiler
