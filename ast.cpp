#include "ast.hpp"
#include "statements.hpp"
#include "traits.hpp"
#include <algorithm>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <stack>
#include <stacktrace>

namespace Compiler {

void Node::unlinkFirstChild() { children.pop_front(); }
Node *Node::getFirstChild() { return children.front(); }

Node *Node::appendChild(Node *nodep) {
  if (nodep != nullptr) {
    children.push_back(nodep);
  } else {
    std::cout << "NULL CHILDREN!" << std::endl;
  }
  return this;
};

// Node *Node::nextChild() {
//   auto node = children[iterationIndex];
//   iterationIndex++;
//   return node;
// }

// void Node::resetIteration() { iterationIndex = 0; }
// bool Node::endsChildIteration() { return iterationIndex == children.size(); }

void Node::walkAllChildlenDFPO(std::function<void(Node *)> callback) {
  std::stack<Node *> s{};
  std::set<Node *> v{};

  s.push(this);

  while (!s.empty()) {
    if (!s.top()->hasNoChild() && !v.contains(s.top())) {
      v.emplace(s.top());
      auto chldrn = s.top()->getChildren();
      for (auto &&ritr = chldrn.rbegin(); ritr != chldrn.rend(); ++ritr) {
        auto &&node = *ritr;
        s.push(node);
      }
    } else {
      callback(s.top());
      s.pop();
    }
  }
}

void Root::printImpl(int depth, Node *node, std::stringstream &ss) {
  for (int i = 0; i < depth; i++) {
    std::cout << "  |";
  }

  std::cout << '-' << node->to_string() << '\n';
  for (auto &&e : node->children) {
    printImpl(depth + 1, e, ss);
  }
}

void Root::print() {
  std::stringstream ss;
  printImpl(0, rootNode, ss);
  // std::cout << ss.str() << std::endl;
}

Root::Root(Root &&src) {
  this->rootNode = src.rootNode;
  src.rootNode = nullptr;
}

std::string Root::to_string() {
  std::stringstream ss;
  printImpl(0, rootNode, ss);
  return ss.str();
}

void Root::gen() {
  if (rootNode != nullptr) {
    rootNode->walkAllChildlenDFPO([&](Node *node) { node->init(); });
    rootNode->gen();
  }
}

Root::Root(Node *root) : rootNode(root) {}
Root::~Root() {
  if (rootNode != nullptr) {
    std::stack<Node *> nodeStack;
    nodeStack.push(rootNode);
    while (true) {
      if (nodeStack.top()->children.size() > 0) {
        nodeStack.push(nodeStack.top()->getFirstChild());
      } else {
        delete nodeStack.top();
        nodeStack.pop();
        if (!nodeStack.empty()) {
          nodeStack.top()->unlinkFirstChild();
        }
      }
      if (nodeStack.empty()) {
        break;
      }
    }
  }
}

void LLVMBuilder::init(contextptr_t ctx, moduleptr_t md) {
  context = ctx;
  llvmModule = md;
  builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
};

Code::moduleptr_t LLVMBuilder::release() { return std::move(llvmModule); }

// block

void Block::gen() {
  auto bb = llvm::BasicBlock::Create(*context, name, parentFunc);
  builder->SetInsertPoint(bb);

  const Type *retType = nullptr;

  walkAllChildlenDFPO([&](Node *n) {
    if (n->isa<Ret>()) {
      auto ret = n->cast<Ret>();
      if (retType != nullptr && ret->returnType() != *retType) {
        throw TypeError(ret->info,
                        std::format("return type missmatching {} vs {}",
                                    retType->name(), ret->returnType().name()));
      } else {
        retType = &ret->returnType();
      }
    }
  });

  llvm::AllocaInst *retptr = nullptr;
  llvm::Type *retTypeInst = nullptr;

  if (retType != nullptr) {
    retTypeInst = retType->getTypeInst();
    retptr = builder->CreateAlloca(retTypeInst, nullptr, "ret");
  }

  auto retbb = llvm::BasicBlock::Create(*context, "return");

  walkAllChildlenDFPO([&](Node *n) {
    if (n->isa<MutableVarDeclaration>()) {
      n->cast<MutableVarDeclaration>()->hosting();
    } else if (n->isa<Ret>()) {
      n->cast<Ret>()->ret2allocaPtr(retptr);
      n->cast<Ret>()->ret2allocaRetBB(retbb);
    }
  });

  cmpStmt.gen();

  walkAllChildlenDFPO([&](Node *n) {
    if (n->isa<Ret>()) {
      n->cast<Ret>()->ret2alloca();
    }
  });

  auto origin = builder->GetInsertBlock();
  retbb->insertInto(parentFunc);

  if (retptr != nullptr && retTypeInst != nullptr) {
    builder->SetInsertPoint(retbb);
    auto retv = builder->CreateLoad(retTypeInst, retptr);
    builder->CreateRet(retv);

  } else {
    builder->SetInsertPoint(origin);
    builder->CreateBr(retbb);
    builder->SetInsertPoint(retbb);
    builder->CreateRet(nullptr);
  }
  builder->SetInsertPoint(origin);
}

} // namespace Compiler
