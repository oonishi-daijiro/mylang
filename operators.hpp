#pragma once

#include "ast.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "traits.hpp"

#include <functional>
#include <initializer_list>
#include <llvm/IR/Operator.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <tuple>
#include <type_traits>
#include <vector>

namespace Compiler {

// // represents operation(invoke, operator , func call ... etc);
// // operation: value... -> value

template <typename... O> class Operation : virtual public Type {
  std::vector<Type *> inputType;

public:
  Operation(std::initializer_list<Type *> inputTypes) : inputType{inputTypes} {}

  virtual ~Operation() = default;
  virtual llvm::Value *operate(O &...) = 0;

  virtual std::tuple<bool, std::string>
  check(const std::vector<Type *> &inputTypes) = 0;

  virtual llvm::Type *resolve(const std::vector<Type *> &inputType) = 0;

  virtual llvm::Type *resolve() override final {
    std::cout << "resolve" << std::endl;
    auto [isValid, wht] = check(inputType);
    if (!isValid) {
      throw TypeError(wht);
    }
    return resolve(inputType);
  };
};

template <typename T>
class UnaryOperator : public Operation<T>, public Expression {
  std::tuple<T &> operand{};

public:
  UnaryOperator(auto *oprnd)
      : Operation<T>{oprnd}, operand{*oprnd}, Expression{oprnd} {}

  virtual void gen() override final { get(); };
  virtual llvm::Value *get() override final {
    auto fn = std::bind_front(&Operation<T>::operate, this);
    return std::apply(fn, operand);
  }
};

template <typename L, typename R>
class BinaryOperator : public Operation<L, R>, public Expression {
  std::tuple<L &, R &> operands;

public:
  BinaryOperator(auto *lv, auto *rv)
      : Operation<L, R>{lv, rv}, operands{*lv, *rv}, Expression{lv, rv} {}

  virtual void gen() override final { get(); }

  virtual llvm::Value *get() override final {
    auto fn = std::bind_front(&Operation<L, R>::operate, this);
    return std::apply(fn, operands);
  };

  virtual std::tuple<bool, std::string>
  check(const std::vector<Type *> &inputType) override final {
    auto loTy = inputType[0];
    auto roTy = inputType[1];
    std::cout << "check" << std::endl;
    if (*loTy != *roTy) {
      return {false,
              std::format("invalid type operand. expected left and "
                          "right operand to be the same type.: {} and {}",
                          loTy->getTypeName(), roTy->getTypeName())};
    } else {
      return {true, {}};
    }
  }
};

class FieldUnaryOperator : public UnaryOperator<Ordered> {
public:
  FieldUnaryOperator(Numeric *v) : UnaryOperator<Ordered>{v} {}

  virtual std::tuple<bool, std::string>
  check(const std::vector<Type *> &inputTypes) override {
    return {true, {}};
  };

  virtual llvm::Type *
  resolve(const std::vector<Type *> &inputType) override final {
    return inputType[0]->resolve();
  }
};

class MinusOperator : public FieldUnaryOperator {
public:
  MinusOperator(Numeric *v) : FieldUnaryOperator{v} {}
  virtual llvm::Value *operate(Ordered &v) override { return v.minus(); }
};

class FieldBinaryOperator : public BinaryOperator<Field, Value> {
public:
  FieldBinaryOperator(Numeric *lv, Expression *rv)
      : BinaryOperator<Field, Value>{lv, rv} {}

  virtual llvm::Type *
  resolve(const std::vector<Type *> &inputType) override final {
    return inputType[0]->resolve();
  }
};

class OrderedBinariOperator : public BinaryOperator<Ordered, Value> {
public:
  OrderedBinariOperator(Numeric *lv, Expression *rv)
      : BinaryOperator<Ordered, Value>{lv, rv} {}

  virtual llvm::Type *
  resolve(const std::vector<Type *> &inputType) override final {
    return inputType[0]->resolve();
  }
};

class AddOperator : public FieldBinaryOperator {
public:
  AddOperator(Numeric *lv, Expression *rv) : FieldBinaryOperator{lv, rv} {}
  virtual llvm::Value *operate(Field &lv, Value &rv) override {
    return lv.add(rv);
  }
};

class Suboperator : public FieldBinaryOperator {
public:
  Suboperator(Numeric *lv, Expression *rv) : FieldBinaryOperator{lv, rv} {}
  virtual llvm::Value *operate(Field &lv, Value &rv) override {
    return lv.sub(rv);
  }
};

class MulOperator : public FieldBinaryOperator {
public:
  MulOperator(Numeric *lv, Expression *rv) : FieldBinaryOperator{lv, rv} {}
  virtual llvm::Value *operate(Field &lv, Value &rv) override {
    return lv.mul(rv);
  }
};

class DivOperator : public FieldBinaryOperator {
public:
  DivOperator(Numeric *lv, Expression *rv) : FieldBinaryOperator{lv, rv} {}
  virtual llvm::Value *operate(Field &lv, Value &rv) override {
    return lv.div(rv);
  }
};

class LeOperator : public OrderedBinariOperator {
public:
  LeOperator(Numeric *lv, Expression *rv) : OrderedBinariOperator{lv, rv} {}
  virtual llvm::Value *operate(Ordered &lv, Value &rv) override {
    return lv.le(rv);
  }
};

class LtOperator : public OrderedBinariOperator {
public:
  LtOperator(Numeric *lv, Expression *rv) : OrderedBinariOperator{lv, rv} {}
  virtual llvm::Value *operate(Ordered &lv, Value &rv) override {
    return lv.lt(rv);
  }
};

class EqOperator : public OrderedBinariOperator {
public:
  EqOperator(Numeric *lv, Expression *rv) : OrderedBinariOperator{lv, rv} {}
  virtual llvm::Value *operate(Ordered &lv, Value &rv) override {
    return lv.eq(rv);
  }
};

class NeOperator : public OrderedBinariOperator {
public:
  NeOperator(Numeric *lv, Expression *rv) : OrderedBinariOperator{lv, rv} {}
  virtual llvm::Value *operate(Ordered &lv, Value &rv) override {
    return lv.ne(rv);
  }
};

class GtOperator : public OrderedBinariOperator {
public:
  GtOperator(Numeric *lv, Expression *rv) : OrderedBinariOperator{lv, rv} {}
  virtual llvm::Value *operate(Ordered &lv, Value &rv) override {
    return lv.gt(rv);
  }
};

class GeOperator : public OrderedBinariOperator {
public:
  GeOperator(Numeric *lv, Expression *rv) : OrderedBinariOperator{lv, rv} {}
  virtual llvm::Value *operate(Ordered &lv, Value &rv) override {
    return lv.ge(rv);
  }
};

}; // namespace Compiler
