#include <functional>
#include <queue>
#include <stack>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>

#include "ast.hpp"

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

void Node::walkAllChildlenDFPO(const std::function<void(Node *)> &callback) {
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

void Node::walkAllChildlenBF(const std::function<void(Node *)> &callback) {
  std::queue<Node *> q{};
  std::set<Node *> v{};

  v.emplace(this);
  q.push(this);

  while (!q.empty()) {
    auto n = q.front();
    q.pop();
    callback(n);
    for (auto &&child : n->children) {
      if (!v.contains(child)) {
        v.emplace(child);
        q.push(child);
      }
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
    rootNode->walkAllChildlenBF([](Node *node) {
      if (node->isa<ScopeSemantic>()) {
        node->cast<ScopeSemantic>()->resolveScope();
      }
    });

    rootNode->walkAllChildlenBF([](Node *node) {
      if (node->isa<SymbolSemantic>()) {
        node->cast<SymbolSemantic>()->resolveSymbol();
      }
    });

    rootNode->walkAllChildlenDFPO([](Node *node) {
      if (node->isa<TypeSemantic>()) {
        node->cast<TypeSemantic>()->resolveType();
      }
    });
    std::cout << "===============  AST  ===============" << std::endl;
    print();

    rootNode->walkAllChildlenDFPO([](Node *node) { node->init(); });
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

void ScopeSemantic::defaultScopeInitalizerImpl(Node *n) {
  if (n->isa<ScopeSemantic>()) {
    auto scpSem = n->cast<ScopeSemantic>();
    scpSem->scope().setParent(this->scope());
  }
  if (n->isa<Symbol>()) {
    auto symbol = n->cast<Symbol>();
    if (n != util::cast<Node>(this)) {
      symbol->setScope(this->scope());
    }
  }
}

} // namespace Compiler
