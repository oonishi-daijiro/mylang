#pragma once
#include <cstdio>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>

#include "ast.hpp"
#include "expressions.hpp"
#include "value.hpp"

namespace Compiler {

class Ret final : public Statement {
private:
  Value &retVal;
  llvm::AllocaInst *retPtr;
  llvm::BasicBlock *retbb;
  llvm::BasicBlock *parentbb;

public:
  Ret(Expression *expr);
  virtual ~Ret() = default;

  virtual void gen() override;
  virtual std::string to_string() override;

  const Type &returnType();
  void ret2allocaPtr(llvm::AllocaInst *ptr);
  void retBlock(llvm::BasicBlock *bb);

  void ret2allocaRetBB(llvm::BasicBlock *blk) { this->retbb = blk; };
  void ret2alloca() {
    auto inst = parentbb->getTerminator();
    auto origin = builder->GetInsertBlock();
    if (inst != nullptr) {
      parentbb->getTerminator()->eraseFromParent();
    }
    builder->SetInsertPoint(parentbb);
    builder->CreateBr(retbb);
    builder->SetInsertPoint(origin);
  }
};

class CompoundStatement : public Statement {
  std::vector<Statement *> stmts;

public:
  CompoundStatement(std::vector<Statement *> &&stmts) : stmts{stmts} {
    for (auto &&stmt : stmts) {
      appendChild(stmt);
    }
  }

  virtual std::string to_string() override;
  virtual void gen() override {
    for (auto &&e : stmts) {
      e->gen();
    }
  }
};

class MutableVarDeclaration final : public Statement {
  Variable *var{nullptr};
  Value &initVal;

public:
  MutableVarDeclaration(const std::string &name, Expression &initVal);
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
