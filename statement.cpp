#include <llvm/IR/BasicBlock.h>
#include <string>

#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "statement.hpp"

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
