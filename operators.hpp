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
  MagmaOperator(Expression *lv, Expression *rv);
  virtual void resolveType() final;
  virtual std::string kind() override = 0;
};

class BinaryOperator : public Operator {
protected:
  Value &lv;
  Value &rv;

public:
  BinaryOperator(Expression *lv, Expression *rv);
  virtual void resolveType() final;
  virtual std::string operationRetTypeName() = 0;
};

class UnaryOperator : public Operator {
protected:
  Value &o;

public:
  UnaryOperator(Expression *o) : Operator{o}, o{*o} {}
  virtual llvm::Value *get() override;
  virtual void resolveType() final override { type.resolve(o.type); };
};

class UnaryAssignOperator : public Operator {
protected:
  Substance *o;

public:
  UnaryAssignOperator(Expression *sub) : Operator{sub} {
    if (!sub->isa<Substance>()) {
      throw TypeError(sub->info, std::format("this value is not assignable"));
    } else {
      o = sub->cast<Substance>();
    }
  }

  virtual void resolveType() { type.resolve(o->type); }
};

class MinusOperator final : public UnaryOperator {
  using UnaryOperator::UnaryOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class IncrementOperator final : public UnaryAssignOperator {
  using UnaryAssignOperator::UnaryAssignOperator;

  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class DecrementOperator final : public UnaryAssignOperator {
  using UnaryAssignOperator::UnaryAssignOperator;

  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class BooleanOperator : public BinaryOperator {
  using BinaryOperator::BinaryOperator;
  virtual std::string operationRetTypeName() final override;
};

class AddOperator final : public MagmaOperator {
  using MagmaOperator::MagmaOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class SubOperator final : public MagmaOperator {
  using MagmaOperator::MagmaOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class MulOperator final : public MagmaOperator {
  using MagmaOperator::MagmaOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class DivOperator final : public MagmaOperator {
  using MagmaOperator::MagmaOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class EqOperator final : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class NeqOperator final : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class LtOperator final : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class LeOperator final : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class GtOperator final : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

class GeOperator final : public BooleanOperator {
  using BooleanOperator::BooleanOperator;
  virtual std::string kind() override;
  virtual llvm::Value *get() override;
};

}; // namespace Compiler
