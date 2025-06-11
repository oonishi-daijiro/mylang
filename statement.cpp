#include <string>

#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "expressions.hpp"
#include "statements.hpp"
#include "traits.hpp"

namespace Compiler {

// // mutable var decl
// MutableVarDecl::MutableVarDecl(const std::string &name, Expression *expr)
//     : var{*new MutableVariable{name, *expr}}, rv{*expr},
//       Statement{&var, expr} {};

// llvm::Value *MutableVarDecl::gen() {
//   var.gen();
//   return var.set(rv.get());
// };

// // constant var decl
// ConstantVarDecl::ConstantVarDecl(const std::string &name, Expression *expr)
//     : var{*new ConstantVariable{name, *expr}}, Statement{&var, expr} {}

// llvm::Value *ConstantVarDecl::gen() { return var.gen(); }

// return
Ret::Ret(Expression *expr) : retVal(*expr), Statement{expr} {};

void Ret::gen() {
  builder->CreateRet(retVal.get());
};

// if statement
// IfStatement::IfStatement(Expression *cond, Block *then, IfStatement *elif)
//     : then{*then}, cond{*cond}, Statement{cond, then, elif} {

//   if (elif != nullptr) {
//     this->elif = elif;
//   }
// }

// void IfStatement::gen() {

//   auto origin = builder->GetInsertBlock();
//   auto parent = origin->getParent();

//   then.setParent(parent);
//   then.gen();
//   auto thenEntry = then.entry();

//   auto isThenEndsRetInst = endsReturnInst();
//   auto cb = blockgen("");
//   builder->SetInsertPoint(origin);
//   builder->CreateCondBr(cond.get(), thenEntry, cb);
//   builder->SetInsertPoint(cb);

//   if (elif) {
//     elif.value()->gen();
//     builder->CreateBr(merge());
//     if (!isThenEndsRetInst) {
//       then.setAsInsertPoint();
//       builder->CreateBr(merge());
//     }
//     builder->SetInsertPoint(merge());
//     return merge();
//   } else {
//     if (!isThenEndsRetInst) {
//       then.setAsInsertPoint();
//       builder->CreateBr(cb);
//     }
//     builder->SetInsertPoint(cb);
//     return cb;
//   }
// };

// // else statement

// llvm::Value *ElseStatement::gen() {
//   auto origin = builder->GetInsertBlock();
//   auto parent = origin->getParent();
//   then.attach(origin);
//   then.setParent(parent);
//   then.gen();

//   auto br = endsReturnInst();
//   then.setAsInsertPoint();

//   if (!br) {
//     builder->CreateBr(merge());
//   }

//   builder->SetInsertPoint(merge());
//   return then.end();
// }
} // namespace Compiler
