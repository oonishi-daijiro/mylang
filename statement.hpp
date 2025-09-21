#pragma once
#include <cstdio>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>

#include "ast.hpp"
#include "expressions.hpp"
#include "scope.hpp"
#include "value.hpp"

namespace Compiler {

class Ret final : public Statement {
private:
  Value *retVal{nullptr};
  llvm::AllocaInst *retPtr;
  llvm::BasicBlock *retbb;
  llvm::BasicBlock *parentbb;

public:
  Ret(Expression *expr);
  Ret();
  virtual ~Ret() = default;

  virtual void gen() override;
  virtual std::string to_string() override;

  const Type &returnType();
  void ret2allocaPtr(llvm::AllocaInst *ptr);
  void retBlock(llvm::BasicBlock *bb);
  void ret2allocaRetBB(llvm::BasicBlock *blk) { this->retbb = blk; };
  void ret2alloca();
};

class CompoundStatement : public Statement {
  std::vector<Statement *> stmts;
  Scope scp{};

public:
  CompoundStatement(std::vector<Statement *> &&stmts) : stmts{stmts} {
    for (auto &&stmt : stmts) {
      appendChild(stmt);
    }
  }

  Scope &scope() { return scp; }

  virtual void resolveScope() override {
    walkAllChildlenBF([&](Node *n) {
      if (n->isa<CompoundStatement>()) {
        auto cmpstmt = n->cast<CompoundStatement>();
        cmpstmt->scope().setParent(scp);
      }
    });
  }

  virtual std::string to_string() override;
  virtual void gen() override {
    for (auto &&e : stmts) {
      e->gen();
    }
  }
};

class MutableLocalVarDeclaration final : public Statement {
  LocalVariable *var{nullptr};
  Value &initVal;

public:
  MutableLocalVarDeclaration(const std::string &name, Expression &initVal);
  virtual void hosting() final;
  virtual void gen() override;
  virtual std::string to_string() override;
};

class Assign final : public Statement {
  Expression &lv;
  Expression &rv;
  Substance *slv{nullptr};

public:
  Assign(Expression *lv, Expression *rv);

  virtual void gen() override;
  virtual std::string to_string() override;
  virtual void init() override;
};

} // namespace Compiler
