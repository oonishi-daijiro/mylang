#include "ast.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "traits.hpp"
#include <llvm/IR/BasicBlock.h>

namespace Compiler {

class IfStatement : public Statement {
  Value &cond;

  Statement &then;
  Statement *els{nullptr};

  llvm::BasicBlock *genbb(const std::string &name = "");

public:
  IfStatement(Expression *cond, Statement *then, Statement *els);

  virtual void gen() override;
  virtual std::string to_string() override;
};

class ForStatement final : public Statement {
  Statement &initStmt;
  Expression &loopCond;
  Expression &next;
  Statement &loopBody;

  llvm::BasicBlock *mergebb{nullptr};
  llvm::BasicBlock *nextbb{nullptr};

public:
  ForStatement(Statement *initStatement, Expression *loopCond,
               Expression *nextExpr, Statement *loopBody);
  ~ForStatement() = default;

  llvm::BasicBlock *genbb(const std::string &name);
  virtual void init() override;

  virtual std::string to_string() override;
  virtual void gen() override;
};

class ContinueStatement : public Statement {
  llvm::BasicBlock *nextbb{nullptr};

public:
  ContinueStatement();
  void setNextbb(llvm::BasicBlock *bb);
  virtual std::string to_string() override;
  virtual void gen() override;
};

class BreakStatement : public Statement {
  llvm::BasicBlock *headerbb{nullptr};

public:
  BreakStatement();
  void setEscapebb(llvm::BasicBlock *bb);
  virtual std::string to_string() override;
  virtual void gen() override;
};

class WhileStatement : public Statement {
  Value &cond;
  Statement &stmt;

public:
  WhileStatement(Expression *cond, Statement *stmt);
  virtual std::string to_string() override;
  virtual void gen() override {};
};

} // namespace Compiler
