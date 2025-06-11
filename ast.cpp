
#include <cctype>
#include <cstring>
#include <deque>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <stack>
#include <string>
#include <utility>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <vector>

#include "ast.hpp"
#include "statements.hpp"

namespace Compiler {

void Node::unlinkFirstChild() { children.pop_front(); }
Node *Node::getFirstChild() { return children.front(); }

Node *Node::appendChild(Node *nodep) {
  if (nodep != nullptr) {
    children.push_back(nodep);
  }
  return this;
};

void Root::printImpl(int depth, Node *node, std::stringstream &ss) {
  for (int i = 0; i < depth; i++) {
    ss << "  |";
  }

  ss << '-' << node->to_string() << '\n';

  for (auto &&e : node->children) {
    printImpl(depth + 1, e, ss);
  }
}

void Root::print() {
  std::stringstream ss;
  printImpl(0, rootNode, ss);
  std::cout << ss.str() << std::endl;
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

void Root::gen() { rootNode->gen(); }

Root::Root(Node *root) : rootNode(root) {}
Root::~Root() {
  if (rootNode != nullptr) {
    std::stack<Node *> nodeStack;
    nodeStack.push(rootNode);
    int indent = 0;
    while (true) {
      if (nodeStack.top()->children.size() > 0) {
        indent++;
        nodeStack.push(nodeStack.top()->getFirstChild());
      } else {
#ifdef SHOW_DELETE_TREE
        for (int i = 0; i < indent; i++) {
          std::cout << "   |";
        }
        std::cout << "delete:" << nodeStack.top()->to_string() << std::endl;
#endif

        delete nodeStack.top();
        nodeStack.pop();
        indent--;
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

void Code::init(contextptr_t ctx, moduleptr_t md) {
  context = ctx;
  llvmModule = md;
  builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
};

Code::moduleptr_t Code::release() { return std::move(llvmModule); }

// block

Block::Block(std::vector<Statement *> &&stmts, std::string name,
             llvm::Function *parent)
    : name{name}, parent{parent}, stmts{stmts} {
  for (auto &&stmt : stmts) {
    appendChild(stmt);
  }
};

void Block::gen() {
  if (entryPtr == nullptr) {
    entryPtr = llvm::BasicBlock::Create(*context, name, parent);
    endPtr = entryPtr;
  }
  builder->SetInsertPoint(entryPtr);

  for (auto &&stmt : stmts) {
    stmt->init();
  }

  for (auto &&stmt : stmts) {
    stmt->gen();
    auto endStmt = builder->GetInsertBlock()->end();

    if (stmt->isa<Ret>()) {
      break;
    } else if (stmt->isa<Block>()) {
      endPtr = llvm::cast<llvm::BasicBlock>(&endStmt);
    }
  }

  for (auto &&stmt : stmts) {
    stmt->finally();
  }
}

void Block::setAsInsertPoint() { builder->SetInsertPoint(endPtr); }

void Block::attach(llvm::BasicBlock *b) {
  entryPtr = b;
  for (auto beg = b->begin(); beg != b->end(); ++beg) {
    if (llvm::isa<llvm::BasicBlock>(beg)) {
      endPtr = llvm::cast<llvm::BasicBlock>(beg);
    }
  }
}

} // namespace Compiler
