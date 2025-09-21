#pragma once

#include "ast.hpp"
#include "block.hpp"
#include "expressions.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <optional>
#include <sstream>

namespace Compiler {

class FunctionSignature {
  std::optional<Type> ret{std::nullopt};
  std::vector<std::pair<std::string, Type>> argument;
  std::vector<Type> argTy;
  Scope scope;

public:
  FunctionSignature(const std::vector<std::pair<std::string, Type>> &&argument,
                    const std::optional<Type> &ret);

  bool operator==(const FunctionSignature &signature) const;

  const std::optional<Type> returnType() const;
  void setInferedReturnType(const Type &t);
  const std::vector<Type> &argType() const;
  std::string to_string();
};

class Function : public Node, public Symbol {
  Block &body;
  FunctionSignature sig;
  std::string name;
  Type inferReturnType();
  Scope scp{};

public:
  Function(const std::string &name, const FunctionSignature &sig, Block *body);
  virtual std::string to_string() override;
  virtual void gen() override;
  virtual void resolveType() override;
  virtual void resolveScope() override;
  virtual void resolveSymbol() override;
  virtual const std::string kind() const override { return "function"; }
};

class FunctionReference : public Expression {};

} // namespace Compiler
