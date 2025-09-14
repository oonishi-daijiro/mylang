#include <string>

#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "expressions.hpp"
#include "statements.hpp"
#include "traits.hpp"

namespace Compiler {

// // constant var decl
// ConstantVarDecl::ConstantVarDecl(const std::string &name, Expression *expr)
//     : var{*new ConstantVariable{name, *expr}}, Statement{&var, expr} {}

// llvm::Value *ConstantVarDecl::gen() { return var.gen(); }

// return
Ret::Ret(Expression *expr) : retVal(*expr), Statement{expr} {};

void Ret::gen() { builder->CreateRet(retVal.get()); };

// void IfStatement::gen() {};
// void ElseStatement::gen() {}
} // namespace Compiler
