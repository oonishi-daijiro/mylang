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
  virtual llvm::Value *get() override = 0;

  virtual void resolveType() = 0;
  virtual void init() override final { resolveType(); };

  virtual std::string to_string() override { return "[***expression***]"; };
};

class DoubleExpr : public Expression {
  double value;

public:
  DoubleExpr(double val) : value{val} {};

  virtual llvm::Value *get() override {
    return llvm::ConstantFP::get(type.getTypeInst(), value);
  };

  virtual void resolveType() override { type.resolve("double"); }

  virtual std::string to_string() override;
};

class IntegerExpr : public Expression {
  int32_t value;

public:
  IntegerExpr(int32_t val) : value{val} {}

  virtual llvm::Value *get() override {
    return llvm::ConstantInt::get(type.getTypeInst(), value);
  };

  virtual void resolveType() override { type.resolve("integer"); }
  virtual std::string to_string() override;
};

class BooleanExpr : public Expression {
  bool value;

public:
  BooleanExpr(bool b) : value{b} {};
  virtual llvm::Value *get() override {
    return llvm::ConstantInt::get(type.getTypeInst(), value);
  };
  virtual void resolveType() override { type.resolve("boolean"); }
  virtual std::string to_string() override;
};

class Substance : public Expression {
public:
  virtual llvm::Value *get() = 0;
  virtual void set(Value &) = 0;
  virtual llvm::Value *ptr() = 0;
};

class Variable : public Substance {
  Type *initialType{nullptr};
  llvm::AllocaInst *pointer{nullptr};
  std::string name;
  static inline std::map<std::string, Variable *> varmap{};

  Variable(const std::string &name, Type *type)
      : name{name}, initialType{type} {}

public:
  static inline Variable *DefineNewVariable(const std::string &name,
                                            Type &type) {
    if (varmap.count(name)) {
      throw SymbolError(std::format("variable {} is already defined", name));
    } else {
      auto ptr = new Variable{name, &type};
      varmap.emplace(name, ptr);
      return ptr;
    }
  }

  Variable(const std::string &name) : name{name} {
    if (!varmap.count(name)) {
      throw SymbolError(std::format("variable \"{}\" is not defined", name));
    } else {
      initialType = varmap.at(name)->initialType;
    }
  }

  virtual std::string to_string() override;

  void allocate() {
    if (pointer == nullptr) {
      pointer = builder->CreateAlloca(type.getTypeInst(), nullptr, name);
    }
  }

  virtual llvm::Value *get() override {
    if (pointer == nullptr) {
      pointer = varmap.at(name)->pointer;
    }
    return builder->CreateLoad(type.getTypeInst(), pointer);
  };

  virtual void set(Value &val) override {
    if (pointer == nullptr) {
      pointer = varmap.at(name)->pointer;
    }
    builder->CreateStore(val.get(), pointer);
  }

  virtual llvm::Value *ptr() override { return pointer; }

  virtual void resolveType() override {
    if (initialType != nullptr) {
      type.resolve(initialType->name());
    } else {
      type.resolve(varmap.at(name)->type.name());
    }
  }
  const std::string &getname() { return name; }
};

} // namespace Compiler
