#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <llvm/IR/DerivedTypes.h>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "errors.hpp"
#include "type.hpp"
#include "type_traits.hpp"
#include "utils.hpp"
#include "value.hpp"

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

class Literal {
public:
  virtual ~Literal() = default;
  virtual std::string value_str() = 0;
};

class DoubleExpr : public Expression, public Literal {
  double value;

public:
  DoubleExpr(double val);

  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;
  virtual std::string value_str() override { return std::to_string(value); }
};

class IntegerExpr : public Expression, public Literal {
  int32_t value;

public:
  IntegerExpr(int32_t val);

  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;

  virtual std::string value_str() override { return std::to_string(value); }
};

class BooleanExpr final : public Expression, public Literal {
  bool value;

public:
  BooleanExpr(bool b);
  virtual llvm::Value *get() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;
  virtual std::string value_str() override { return std::to_string(value); }
};

class ArrayExpr final : public Expression, public Literal {
  std::vector<Value *> elements{};
  std::stringstream valstr{};
  llvm::Value *head{nullptr};
  
  public:
  ArrayExpr(std::vector<Expression *> &&initExpr);
  Type elementTy;

  virtual llvm::Value *get() override {
    auto arraysize = builder->getInt64(elements.size());
    auto arrayTy =
        llvm::ArrayType::get(elementTy.getTypeInst(), elements.size());
    head = builder->CreateAlloca(arrayTy, arraysize);
    auto zero = builder->getInt64(0);

    for (size_t i = 0; i < elements.size(); i++) {
      auto index = builder->getInt64(i);
      auto p = builder->CreateGEP(elementTy.getTypeInst(), head, {index});
      builder->CreateStore(elements[i]->get(), p);
    }
    return head;
  };

  virtual void resolveType() override {
    if (elements.size() > 0) {
      bool isAllSameTy = true;
      for (int i = 0; i < elements.size() - 1; i++) {
        isAllSameTy &= elements[i]->type == elements[i + 1]->type;
      }

      if (!isAllSameTy) {
        throw TypeError(this->info, "array element must all same type");
      }
      auto &ty = elements[0]->type;
      elementTy = elements[0]->type;
      auto arrayPointerTy = llvm::PointerType::get(elementTy.getTypeInst(), 0);
      type = Type{std::format("@array[{}]", elements.size()), arrayPointerTy,
                  new ArrayTyTrait{}};
    } else {
      throw TypeError(this->info,
                      "array expression must be have 1 or more elements");
    }
  };

  virtual std::string to_string() override {
    return std::format("Array: {}:[{}]", type.name(), valstr.str());
  };

  virtual std::string value_str() override { return valstr.str(); }
};

class StringExpr final : public Expression, public Literal {
  std::string value;

public:
  StringExpr(const std::string &str) : value{str} {}
  virtual llvm::Value *get() override {};
  virtual std::string to_string() override {};
  virtual void resolveType() override {};
  virtual std::string value_str() override {
    return std::format("\"{}\"", value);
  }
};

class Substance : public virtual Expression {
public:
  virtual ~Substance() = default;
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
