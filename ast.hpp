#pragma once

#include <cctype>
#include <cstring>
#include <deque>
#include <format>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "debug.hpp"
#include "errors.hpp"
#include "token.hpp"
#include "traits.hpp"

namespace Compiler {

class Code {
public:
  using contextptr_t = llvm::LLVMContext *;
  using moduleptr_t = llvm::Module *;
  using irbuilderptr_t = std::unique_ptr<llvm::IRBuilder<>>;

protected:
  static inline contextptr_t context;
  static inline moduleptr_t llvmModule;
  static inline irbuilderptr_t builder;

public:
  virtual ~Code() = default;
  Code() = default;

  static void init(contextptr_t context, moduleptr_t module);
  static moduleptr_t release();

  virtual void gen() = 0;
  template <typename T>
  bool isa()
    requires(std::is_base_of_v<Code, T>)
  {
    auto ptr = dynamic_cast<T *>(this);
    return ptr != nullptr;
  };
  template <typename T>
  T *cast()
    requires(std::is_base_of_v<Code, T>)
  {
    if (!isa<T>()) {
      throw CastError(
          std::format("{} is not {}", this->to_string(), typeid(T).name()));
    }
    return dynamic_cast<T *>(this);
  }

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

public:
  const DebugInfo info;

  virtual ~Node() = default;
  Node(auto &&...child) : info{(*curtok)->info} { (appendChild(child), ...); }
  bool empty() { return children.empty(); }

  static inline void init(tokitr_t &itr) { curtok = &itr; }
};

class Program : public Node {
  using Node::Node;
};

class Statement : public Node {
public:
  using Node::Node;
  virtual void init() {}
  virtual void finally() {}
};

// Block(s)
class Block : public Node {
  llvm::Function *parent{nullptr};
  llvm::BasicBlock *entryPtr{nullptr};
  llvm::BasicBlock *endPtr{nullptr};

  std::string name;
  bool generated = false;

  std::vector<Statement *> stmts;

public:
  // remove name and parent constructor after.
  Block(std::vector<Statement *> &&stmts, std::string name = "",
        llvm::Function *parentFunc = nullptr);
  Block() = delete;
  virtual ~Block() = default;

  llvm::BasicBlock *entry() { return entryPtr; };
  llvm::BasicBlock *end() { return endPtr; };

  void setAsInsertPoint();

  void setName(std::string_view name) { this->name = name; }
  void setParent(llvm::Function *func) { parent = func; }
  void attach(llvm::BasicBlock *b);

  virtual void gen() override;
  virtual std::string to_string() override;
};

class Expression : public Statement, public Value {
public:
  using Statement::Statement;
  virtual ~Expression() = default;

  virtual void gen() override { get(); };
  virtual llvm::Value *get() override = 0;
  virtual std::string to_string() override { return "[***expression***]"; };
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
