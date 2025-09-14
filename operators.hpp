#pragma once

#include "ast.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "traits.hpp"

#include <llvm/IR/Operator.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace Compiler {

class Operator : public Expression {
  using Expression::Expression;
  virtual std::string to_string() final;
  virtual std::string kind() = 0;
};

class MagmaOperator : public Operator {
protected:
  Value &lv;
  Value &rv;

public:
  MagmaOperator(Expression *lv, Expression *rv)
      : lv{*lv}, rv{*rv}, Operator{lv, rv} {}

  virtual void resolveType() final {
    if (lv.type != rv.type) {
      throw TypeError(this->info, std::format("type mismatch: '{}' vs '{}'",
                                              lv.type.name(), rv.type.name()));
    } else {
      type.resolve(lv.type.name());
    }
  };

  virtual std::string kind() override = 0;
};

class BinaryOperator : public Operator {
protected:
  Value &lv;
  Value &rv;

public:
  BinaryOperator(Expression *lv, Expression *rv)
      : lv{*lv}, rv{*rv}, Operator{lv, rv} {}
  virtual void resolveType() final {
    if (lv.type != rv.type) {
      throw TypeError(this->info, std::format("type mismatch: '{}' vs '{}'",
                                              lv.type.name(), rv.type.name()));
    } else {
      type.resolve(operationRetTypeName());
    }
  }
  virtual std::string operationRetTypeName() = 0;
};

class UnaryOperator : public Operator {
protected:
  Value &o;

public:
  UnaryOperator(Expression *o) : Operator{o}, o{*o} {}
  virtual llvm::Value *get() override;
  virtual void resolveType() final override { type.resolve(o.type.name()); };
};

class MinusOperator : public UnaryOperator {
  using UnaryOperator::UnaryOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class BooleanOperator : public BinaryOperator {
  using BinaryOperator::BinaryOperator;
  virtual std::string operationRetTypeName() final override;
};

class AddOperator : public MagmaOperator {
  using MagmaOperator::MagmaOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class SubOperator : public MagmaOperator {
  using MagmaOperator::MagmaOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class MulOperator : public MagmaOperator {
  using MagmaOperator::MagmaOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class DivOperator : public MagmaOperator {
  using MagmaOperator::MagmaOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class EqOperator : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class NeqOperator : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class LtOperator : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class LeOperator : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class GtOperator : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class GeOperator : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

}; // namespace Compiler
