#include <llvm/IR/BasicBlock.h>
#include <string>

#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "statement.hpp"

namespace Compiler {

// MutableVarDeclaration
MutableLocalVarDeclaration::MutableLocalVarDeclaration(const std::string &name,
                                                       Expression &initVal)
    : initVal{initVal}, var{new LocalVar(name, initVal.type)} {
  appendChild(&initVal);
  appendChild(var);
}

void MutableLocalVarDeclaration::hosting() { var->allocate(); }
void MutableLocalVarDeclaration::gen() { var->set(initVal); }

// assign

Assign::Assign(Expression *lv, Expression *rv)
    : lv{*lv}, rv{*rv}, Statement{lv, rv} {};

void Assign::gen() {
  if (lv.type != rv.type) {
    throw TypeError(this->info, std::format("type missmatching {} vs {}",
                                            lv.type.name(), rv.type.name()));
  }
  slv->set(rv);
}

void Assign::init() {
  if (!lv.isa<Substance>()) {
    throw SyntaxError(info, std::format("cannot assign value to rvalue of {}",
                                        lv.to_string()));
  } else {
    slv = lv.cast<Substance>();
  }
}
// return
Ret::Ret(Expression *expr) : retVal(expr), Statement{expr} {};
Ret::Ret() {}
void Ret::ret2allocaPtr(llvm::AllocaInst *ptr) { retPtr = ptr; }

void Ret::gen() {
  parentbb = builder->GetInsertBlock();
  if (retPtr != nullptr) {
    builder->CreateStore(retVal->get(), retPtr);
  }
};

const Type &Ret::returnType() {
  if (retVal != nullptr) {
    return retVal->type;
  } else {
    return Type::GetType("void");
  }
};

void Ret::ret2alloca() {
  auto terminator = parentbb->getTerminator();
  auto origin = builder->GetInsertBlock();
  if (terminator != nullptr) {
    parentbb->getTerminator()->eraseFromParent();
  }
  builder->SetInsertPoint(parentbb);
  builder->CreateBr(retbb);
  builder->SetInsertPoint(origin);
}

// compound statement

CompoundStatement::CompoundStatement(std::vector<Statement *> &&s)
    : stmts{std::move(s)} {
  for (auto &&stmt : stmts) {
    appendChild(stmt);
  }
}

void CompoundStatement::resolveScope() {
  walkAllChildlenBF(defaultScopeInitalizer);
}

void CompoundStatement::gen() {
  for (auto &&e : stmts) {
    e->gen();
  }
}

} // namespace Compiler
