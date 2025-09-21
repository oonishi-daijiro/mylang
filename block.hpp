#pragma once

#include "ast.hpp"
#include "type.hpp"

namespace Compiler {
class Block : public Node {
  Statement &cmpStmt;
  llvm::Function *parentFunc;
  std::string name;
  Type returnType{};
  
public:
  Block(Statement *cmpStmt) : Node{cmpStmt}, cmpStmt{*cmpStmt} {}

  void setParentFunc(llvm::Function *func);
  void setReturnType(const Type &);

  virtual void gen() override;
  virtual std::string to_string() override;
};
} // namespace Compiler
