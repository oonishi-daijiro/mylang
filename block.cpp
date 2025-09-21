#include "block.hpp"
#include "statement.hpp"
#include "type.hpp"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>

namespace Compiler {
// block
void Block::setParentFunc(llvm::Function *f) { parentFunc = f; }
void Block::setReturnType(const Type &type) { returnType = type; }

void Block::gen() {
  auto bb = llvm::BasicBlock::Create(*context, name, parentFunc);

  builder->SetInsertPoint(bb);
  auto retbb = llvm::BasicBlock::Create(*context, "return");
  llvm::AllocaInst *retptr = nullptr;
  if (returnType != "void") {
    retptr = builder->CreateAlloca(returnType.getTypeInst(), nullptr, "ret");
  }

  walkAllChildlenDFPO([&](Node *n) {
    if (n->isa<MutableLocalVarDeclaration>()) {
      n->cast<MutableLocalVarDeclaration>()->hosting();
    } else if (n->isa<Ret>()) {
      n->cast<Ret>()->ret2allocaPtr(retptr);
      n->cast<Ret>()->ret2allocaRetBB(retbb);
    }
  });

  cmpStmt.gen();
  auto origin = builder->GetInsertBlock();
  builder->CreateBr(retbb);

  walkAllChildlenDFPO([&](Node *n) {
    if (n->isa<Ret>()) {
      n->cast<Ret>()->ret2alloca();
    }
  });

  retbb->insertInto(parentFunc);
  builder->SetInsertPoint(retbb);

  if (returnType != "void" && retptr != nullptr) {
    auto retVal = builder->CreateLoad(returnType.getTypeInst(), retptr);
    builder->CreateRet(retVal);
  } else {
    builder->CreateRetVoid();
  }
}
} // namespace Compiler
