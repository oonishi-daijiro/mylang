#include <llvm/IR/BasicBlock.h>
#include <string>

#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "expressions.hpp"
#include "statements.hpp"
#include "traits.hpp"

namespace Compiler {

// MutableVarDeclaration
MutableVarDeclaration::MutableVarDeclaration(const std::string &name,
                                             Expression &initVal)
    : initVal{initVal} {
  var = Variable::DefineNewVariable(name, initVal.type);
  appendChild(&initVal);
  appendChild(var);
}

void MutableVarDeclaration::hosting() { var->allocate(); }
void MutableVarDeclaration::gen() { var->set(initVal); }

// assign

Assign::Assign(Substance *lv, Expression *rv)
    : lv{*lv}, rv{*rv}, Statement{lv, rv} {};

void Assign::gen() {
  if (lv.type != rv.type) {
    throw TypeError(this->info, std::format("type missmatching {} vs {}",
                                            lv.type.name(), rv.type.name()));
  }
  lv.set(rv);
}

// return
Ret::Ret(Expression *expr) : retVal(*expr), Statement{expr} {};
void Ret::ret2allocaPtr(llvm::AllocaInst *ptr) { retPtr = ptr; }

void Ret::gen() {
  parentbb = builder->GetInsertBlock();

  if (retPtr != nullptr) {
    builder->CreateStore(retVal.get(), retPtr);
  } else {
    builder->CreateRet(retVal.get());
  }
};
const Type &Ret::returnType() { return retVal.type; };

} // namespace Compiler
