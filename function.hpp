#pragma once

#include "ast.hpp"
#include "block.hpp"
#include "expressions.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "value.hpp"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <optional>

#include <sstream>

namespace Compiler {

class ArgumentInfo {
  std::string n;
  Type ty;

public:
  const Type &type() const { return ty; }
  const std::string &name() const { return n; }
  ArgumentInfo(const std::string &name, const Type &ty) : n{name}, ty{ty} {}
};

class FunctionSignature {
  std::optional<Type> ret{std::nullopt};
  std::vector<ArgumentInfo> args;
  std::vector<const Type *> argTy;

public:
  FunctionSignature(const std::vector<ArgumentInfo> &&argument,
                    const std::optional<Type> &ret);

  bool operator==(const FunctionSignature &signature) const;

  const std::optional<Type> returnType() const;
  const decltype(args) &arguments() const;
  void setInferedReturnType(const Type &t);

  const std::vector<const Type *> &argType() const;
  const std::string to_string() const;
};

class FunctionArgument : public Variable {
  llvm::Value *argSubstance{nullptr};
  llvm::AllocaInst *argCopy{nullptr};
  std::string name;
  Type initTy;

public:
  FunctionArgument(const std::string &name, const Type &type);

  virtual void resolveSymbol() override;
  virtual void resolveType() override;
  virtual std::string to_string() override;
  virtual llvm::Value *get() override;
  virtual void set(Value &) override;
  virtual llvm::Value *ptr() override;
  virtual const std::string kind() const override;

  void setArgSubstance(llvm::Value *);
  void arg2local() {
    argCopy =
        builder->CreateAlloca(initTy, nullptr, std::format("arg_{}", name));
    builder->CreateStore(argSubstance, argCopy);
  }
};

class Function : public Node,
                 public Value,
                 public Substance<Immutable>,
                 public Symbol,
                 public SymbolSemantic,
                 public TypeSemantic,
                 public ScopeSemantic {
  FunctionSignature sig;
  Block &body;
  std::string name{""};
  llvm::Function *func{nullptr};
  std::vector<FunctionArgument *> argments{};
  Type inferReturnType();

public:
  Function(const std::string &name, const FunctionSignature &sig, Block *body);

  virtual std::string to_string() override;
  virtual void gen() override;
  virtual void resolveScope() override;
  virtual void resolveSymbol() override;
  virtual void resolveType() override;
  virtual void init() override;

  virtual llvm::Value *get() override { return func; };
  virtual llvm::Value *ptr() override { return func; };

  virtual const std::string kind() const override { return "function"; }
};

} // namespace Compiler
