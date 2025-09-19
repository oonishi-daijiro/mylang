#pragma once

#include <cctype>
#include <cstring>
#include <deque>
#include <format>
#include <functional>
#include <initializer_list>
#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "debug.hpp"
#include "token.hpp"
// #include "traits.hpp"

namespace Compiler {

class LLVMBuilder {
public:
  virtual ~LLVMBuilder() = default;
  using contextptr_t = llvm::LLVMContext *;
  using moduleptr_t = llvm::Module *;
  using irbuilderptr_t = std::unique_ptr<llvm::IRBuilder<>>;

protected:
  static inline contextptr_t context;
  static inline moduleptr_t llvmModule;
  static inline irbuilderptr_t builder;

public:
  static void init(contextptr_t context, moduleptr_t module);
  static moduleptr_t release();
};

class Code : public LLVMBuilder {
public:
  virtual ~Code() = default;
  Code() = default;

  template <typename T> bool isa() {
    auto ptr = dynamic_cast<T *>(this);
    return ptr != nullptr;
  };
  template <typename T> T *cast() {
    if (!isa<T>()) {
      throw std::runtime_error(
          std::format("{} is not {}", this->to_string(), typeid(T).name()));
    }
    return dynamic_cast<T *>(this);
  }

  virtual void gen() = 0;
  virtual std::string to_string() = 0;
};

template <typename T, typename P>
concept ptrof = requires() {
  !std::is_pointer_v<T>;
  std::is_pointer_v<P>;
  std::is_same_v<T *, P>;
};

class Node : public Code {
  friend class Root;
  virtual void unlinkFirstChild() final;
  virtual Node *getFirstChild() final;

  using tokitr_t = std::vector<Token>::const_iterator;
  static inline tokitr_t *curtok;
  std::deque<Node *> children{};

protected:
  virtual Node *appendChild(Node *nodep) final;
  std::deque<Node *> &getChildren() { return children; };

public:
  const DebugInfo info;
  virtual ~Node() = default;

  template <typename... Children>
    requires(
        std::is_base_of_v<
            Code, std::remove_pointer_t<std::remove_reference_t<Children>>> &&
        ...)
  Node(Children &&...child) : info{(*curtok)->info} {
    (appendChild(child), ...);
  }

  Node(std::initializer_list<Node *> init) : info{(*curtok)->info} {
    for (auto &&e : init) {
      appendChild(e);
    }
  }

  bool hasNoChild() { return children.empty(); }
  virtual void walkAllChildlenDFPO(std::function<void(Node *)>) final;
  virtual void init() {}

  static inline void init(tokitr_t &itr) { curtok = &itr; }
};

class Program : public Node {
  using Node::Node;
};

class Statement : public Node {
public:
  using Node::Node;
};

// Block(s)
class Block : public Node {
  Statement &cmpStmt;
  llvm::Function *parentFunc;
  std::string name;

public:
  Block(Statement *cmpStmt, const std::string &name = "",
        llvm::Function *parentFunc = nullptr)
      : Node{cmpStmt}, cmpStmt{*cmpStmt}, parentFunc{parentFunc}, name{name} {}

  virtual void gen() override;
  virtual std::string to_string() override;
};

class Root : public Code {
  Node *rootNode;
  void printImpl(int depth, Node *node, std::stringstream &ss);

public:
  void print();
  void gen() override;
  virtual std::string to_string() override;

  Root(const Root &) = delete;
  Root(Root &&src);
  Root(Node *rootNode);
  Root(Code *rootExpr);

  virtual ~Root();
};

} // namespace Compiler
