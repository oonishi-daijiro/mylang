#include <llvm/IR/BasicBlock.h>

#include "ast.hpp"
#include "control_statements.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "utils.hpp"

namespace Compiler {

IfStatement::IfStatement(Expression *cond, Statement *then, Statement *els)
    : Statement{cond, then}, cond{*cond}, then{*then}, els{els} {
  if (els != nullptr) {
    appendChild(els);
  }
}

llvm::BasicBlock *IfStatement::genbb(const std::string &name) {
  return llvm::BasicBlock::Create(*context, name,
                                  builder->GetInsertBlock()->getParent());
};

void IfStatement::gen() {
  auto origin = builder->GetInsertBlock();
  auto condVal = cond.get();

  auto thenbb = genbb("then");
  builder->SetInsertPoint(thenbb);
  then.gen();
  auto thenEndBlock = builder->GetInsertBlock();

  if (els != nullptr) {
    auto elsbb = genbb("else");
    builder->SetInsertPoint(elsbb);
    els->gen();
    auto m = genbb("merge-if");
    builder->CreateBr(m);
    builder->SetInsertPoint(origin);
    builder->CreateCondBr(condVal, thenbb, elsbb);
    builder->SetInsertPoint(thenEndBlock);
    if (thenEndBlock->getTerminator() == nullptr) {
      builder->CreateBr(m);
    }
    builder->SetInsertPoint(m);
  } else {
    auto m = genbb("merge-if");
    builder->SetInsertPoint(thenEndBlock);
    if (thenEndBlock->getTerminator() == nullptr) {
      builder->CreateBr(m);
    }
    builder->SetInsertPoint(origin);
    builder->CreateCondBr(condVal, thenbb, m);
    builder->SetInsertPoint(m);
  }
};

void IfStatement::resolveType() {
  if (cond.type != "boolean") {
    throw TypeError(info,
                    std::format("condition expression should be boolean but {}",
                                cond.type.name()));
  }
}

// for statement

ForStatement::ForStatement(Statement *initStatement, Expression *loopCond,
                           Expression *nextExpr, Statement *loopBody)
    : Statement{initStatement, loopCond, nextExpr, loopBody},
      initStmt{*initStatement}, loopCond{*loopCond}, next{*nextExpr},
      loopBody{*loopBody} {}

void ForStatement::init() {
  mergebb = llvm::BasicBlock::Create(*context, "merge-for");
  nextbb = llvm::BasicBlock::Create(*context, "next-for");

  this->walkAllChildlenDFPO([&](Node *n) {
    if (n->isa<ContinueStatement>()) {
      n->cast<ContinueStatement>()->setNextbb(nextbb);
    } else if (n->isa<BreakStatement>()) {
      n->cast<BreakStatement>()->setEscapebb(mergebb);
    }
  });
};

void ForStatement::resolveType() {
  if (loopCond.type.name() != "boolean") {
    throw TypeError(info, std::format("condition must be boolean but: {}",
                                      loopCond.type.name()));
  }
}

void ForStatement::resolveScope() { walkAllChildlenBF(defaultScopeInitalizer); }

llvm::BasicBlock *ForStatement::genbb(const std::string &name) {
  return llvm::BasicBlock::Create(*context, name,
                                  builder->GetInsertBlock()->getParent());
};

void ForStatement::gen() {
  auto originbb = builder->GetInsertBlock();
  auto headerbb = genbb("for-header");
  auto bodybb = genbb("for-body");

  builder->SetInsertPoint(originbb);
  initStmt.gen();
  builder->CreateBr(headerbb);

  builder->SetInsertPoint(headerbb);
  builder->CreateCondBr(loopCond.get(), bodybb, mergebb);

  builder->SetInsertPoint(bodybb);
  loopBody.gen();

  nextbb->insertInto(builder->GetInsertBlock()->getParent());
  builder->CreateBr(nextbb);

  builder->SetInsertPoint(nextbb);
  next.gen();
  builder->CreateBr(headerbb);

  mergebb->insertInto(builder->GetInsertBlock()->getParent());

  builder->SetInsertPoint(mergebb);
};

WhileStatement::WhileStatement(Expression *cond, Statement *stmt)
    : Statement{cond, stmt}, cond{*cond}, whileBody{*stmt} {}

llvm::BasicBlock *WhileStatement::genbb(const std::string &name) {
  return llvm::BasicBlock::Create(*context, name,
                                  builder->GetInsertBlock()->getParent());
};

void WhileStatement::resolveType() {
  if (cond.type != "boolean") {
    throw TypeError(info, std::format("condition must be boolean but: {}",
                                      cond.type.name()));
  }
}

void WhileStatement::init() {
  mergebb = llvm::BasicBlock::Create(*context, "while-merge");
  condbb = llvm::BasicBlock::Create(*context, "while-cond");

  walkAllChildlenDFPO([&](Node *n) {
    if (n->isa<BreakStatement>()) {
      n->cast<BreakStatement>()->setEscapebb(mergebb);
    } else if (n->isa<ContinueStatement>()) {
      n->cast<ContinueStatement>()->setNextbb(condbb);
    }
  });
}

void WhileStatement::gen() {
  auto parent = builder->GetInsertBlock()->getParent();
  builder->CreateBr(condbb);

  auto whilebodybb = genbb("while-body");
  condbb->insertInto(parent);
  mergebb->insertInto(parent);

  builder->SetInsertPoint(condbb);
  builder->CreateCondBr(cond.get(), whilebodybb, mergebb);

  builder->SetInsertPoint(whilebodybb);
  whileBody.gen();
  builder->CreateBr(condbb);

  builder->SetInsertPoint(mergebb);
}

ContinueStatement::ContinueStatement() : Statement{} {}
void ContinueStatement::setNextbb(llvm::BasicBlock *bb) {
  if (nextbb == nullptr) {
    nextbb = bb;
  }
}

void ContinueStatement::gen() {
  if (nextbb != nullptr) {
    builder->CreateBr(nextbb);
  } else {
    throw SyntaxError(
        this->info,
        std::format("continute statement must be inside for/while statement."));
  }
}

BreakStatement::BreakStatement() : Statement{} {}

void BreakStatement::setEscapebb(llvm::BasicBlock *bb) {
  if (headerbb == nullptr) {
    headerbb = bb;
  }
}

void BreakStatement::gen() {
  if (headerbb != nullptr) {
    builder->CreateBr(headerbb);
  } else {
    throw SyntaxError(
        this->info,
        std::format("break statement must be inside for/while statement"));
  }
}

}; // namespace Compiler
