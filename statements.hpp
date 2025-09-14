#pragma once

#include "ast.hpp"
#include "errors.hpp"
#include "token.hpp"
#include "traits.hpp"
#include <llvm/IR/BasicBlock.h>
#include <optional>

#include "expressions.hpp"
namespace Compiler {

class Ret : public Statement {
private:
  Value &retVal;

public:
  Ret(Expression *expr);
  virtual ~Ret() = default;

  virtual void gen() override;
  virtual std::string to_string() override;
};

class MutableVarDeclaration : public Statement {
  Variable *var{nullptr};
  Value &initVal;

public:
  MutableVarDeclaration(const std::string &name, Expression &initVal)
      : initVal{initVal} {
    var = Variable::DefineNewVariable(name, initVal.type);
    appendChild(&initVal);
    appendChild(var);
  }

  void hosting() { var->allocate(); }

  virtual void gen() override {
    var->allocate();
    var->set(initVal);
  }

  virtual std::string to_string() override;
};

class Assign : public Statement {
  Substance &lv;
  Expression &rv;

public:
  Assign(Substance *lv, Expression *rv)
      : lv{*lv}, rv{*rv}, Statement{lv, rv} {};

  virtual void gen() override {
    if (lv.type != rv.type) {
      throw TypeError(this->info, std::format("type missmatching {} vs {}",
                                              lv.type.name(), rv.type.name()));
    }
    lv.set(rv);
  }
  virtual std::string to_string() override;
};

class IfStatement : public Statement {
  Value &cond;
  Statement &then;
  Statement *els{nullptr};

  llvm::BasicBlock *genbb(const std::string &name = "") {
    return llvm::BasicBlock::Create(*context, name,
                                    builder->GetInsertBlock()->getParent());
  }

public:
  IfStatement(Expression *cond, Statement *then, Statement *els)
      : Statement{cond, then}, cond{*cond}, then{*then}, els{els} {
    if (els != nullptr) {
      appendChild(els);
    }
  }

  virtual void gen() override {
    auto origin = builder->GetInsertBlock();
    auto condVal = cond.get();

    auto thenbb = genbb("");
    builder->SetInsertPoint(thenbb);
    then.gen();
    auto thenEndBlock = builder->GetInsertBlock();

    if (els != nullptr) {
      auto elsbb = genbb("");
      builder->SetInsertPoint(elsbb);
      els->gen();
      auto m = genbb("");
      builder->CreateBr(m);
      builder->SetInsertPoint(origin);
      builder->CreateCondBr(condVal, thenbb, elsbb);
      builder->SetInsertPoint(thenEndBlock);
      builder->CreateBr(m);
      builder->SetInsertPoint(m);
    } else {
      auto m = genbb("");
      builder->SetInsertPoint(thenEndBlock);
      builder->CreateBr(m);
      builder->SetInsertPoint(origin);
      builder->CreateCondBr(condVal, thenbb, m);
      builder->SetInsertPoint(m);
    }
  };

  virtual std::string to_string() override;
};

class ElseStatement : public IfStatement {
public:
};

} // namespace Compiler
