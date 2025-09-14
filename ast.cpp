
#include <cctype>
#include <cstring>
#include <deque>
#include <functional>
#include <iostream>
#include <llvm/IR/BasicBlock.h>
#include <memory>
#include <new>
#include <ostream>
#include <queue>
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
  } else {
    std::cout << "NULL CHILDREN!" << std::endl;
  }
  return this;
};
Node *Node::nextChild() {
  auto node = children[iterationIndex];
  iterationIndex++;
  return node;
}

void Node::resetIteration() { iterationIndex = 0; }
bool Node::endsChildIteration() { return iterationIndex == children.size(); }

void Node::walkAllChildlenDFPO(std::function<void(Node *)> callback) {
  std::stack<Node *> nodeStack;
  std::vector<Node *> allChildren{};

  nodeStack.push(this);

  while (true) {
    if (!nodeStack.top()->endsChildIteration()) {
      nodeStack.push(nodeStack.top()->nextChild());
    } else {
      allChildren.emplace_back(nodeStack.top());
      callback(nodeStack.top());
      nodeStack.pop();
    }
    if (nodeStack.empty()) {
      break;
    }
  }

  for (auto &&child : allChildren) {
    child->resetIteration();
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
  cmpStmt.gen();
}

} // namespace Compiler
