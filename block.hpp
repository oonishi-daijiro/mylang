#pragma once

#include "ast.hpp"
#include "type.hpp"

namespace Compiler {
class Block : public Node {
  std::vector<Statement *> stmts;
  llvm::Function *parentFunc;
  std::string name;
  Type returnType{};

public:
  Block(std::vector<Statement *> &&stmts);
  void setParentFunc(llvm::Function *func);
  void setReturnType(const Type &);

  virtual void gen() override;
  virtual std::string to_string() override;
};
} // namespace Compiler
