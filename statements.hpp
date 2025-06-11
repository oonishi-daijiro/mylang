#pragma once

#include "ast.hpp"
#include "expressions.hpp"
#include "traits.hpp"
#include <optional>

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

class IfStatement : public Statement {
protected:
  Expression &cond;
  Block &then;
  std::optional<IfStatement *> elif{std::nullopt};
  llvm::BasicBlock *mb{nullptr};

public:
  IfStatement() = delete;
  IfStatement(Expression *cond, Block *then, IfStatement *elif);

  virtual ~IfStatement() = default;

  llvm::BasicBlock *blockgen(std::string name = "") {
    return llvm::BasicBlock::Create(*context, name,
                                    builder->GetInsertBlock()->getParent());
  }

  llvm::BasicBlock *merge() {
    if (mb == nullptr) {
      mb = blockgen("");
    }
    return mb;
  }

  bool endsReturnInst() {
    auto block = builder->GetInsertBlock();
    if (!block->empty()) {
      return llvm::isa<llvm::ReturnInst>(block->back());
    } else {
      return false;
    }
  }

  virtual void gen() override;
  virtual std::string to_string() override;
};

class ElseStatement : public IfStatement {

public:
  ElseStatement(Block *then) : IfStatement(nullptr, then, nullptr) {}
  virtual ~ElseStatement() = default;

  virtual void gen() override;
  virtual std::string to_string() override;
};


} // namespace AST
