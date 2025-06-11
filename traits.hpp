#pragma once

#include "errors.hpp"
#include <format>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Type.h>
#include <map>
#include <string>

#include <iostream>
#include <llvm/IR/Value.h>
#include <type_traits>

namespace Compiler {

class Code;
template <typename T>
concept NOPTR_NOREF = requires() {
  !std::is_pointer_v<T>;
  !std::is_reference_v<T>;
};

// template <typename T>
// concept LRV = requires() {
//   std::is_base_of_v<LValue, T> || std::is_base_of_v<RValue, T>;
//   requires NOPTR_NOREF<T>;
// };

template <typename T>
concept CODE = requires() {
  std::is_base_of_v<Code, T>;
  requires NOPTR_NOREF<T>;
};

class Type {
  static inline std::map<std::string, llvm::Type *> typeTable;

public:
  static inline llvm::Type *DefineNewType(const std::string &name,
                                          llvm::Type *type) {

    if (typeTable.count(name)) {
      throw std::format("type {} is already defined.", name);
    }

    typeTable[name] = type;
    return type;
  }

  static inline llvm::Type *GetType(const std::string &name) {
    if (!typeTable.count(name)) {
      throw std::format("type {} is not defined.", name);
    }

    return typeTable[name];
  }

private:
  llvm::Type *inst;
  std::string name;

public:
  Type(const std::string &name) : inst{GetType(name)}, name{name} {}
  Type(const Type &rt) : inst{rt.inst}, name{rt.name} {}

  Type() : inst{nullptr}, name{"depend_type"} {} // a type that depends on code.
  virtual ~Type() = default;

  virtual llvm::Type *resolve() {
    if (inst == nullptr) {
      throw TypeError("cannot resolve dependent type");
    }
    return inst;
  }

  const std::string &getTypeName() const { return name; };
  bool operator==(Type &rv) { return name == rv.name; }
  bool operator!=(Type &rv) { return !(*this == rv); }
};

template <typename T>
concept type_cp_t = requires() { std::is_base_of_v<Type, T>; };

class Value : virtual public Type {
public:
  virtual ~Value() = default;
  virtual llvm::Value *get() = 0;
};

class Field : public Value {
public:
  virtual ~Field() = default;

  virtual llvm::Value *add(Value &rv) = 0;
  virtual llvm::Value *sub(Value &rv) = 0;
  virtual llvm::Value *mul(Value &rv) = 0;
  virtual llvm::Value *div(Value &rv) = 0;
};

class Ordered : public Field {
public:
  virtual ~Ordered() = default;
  virtual llvm::Value *minus() = 0;
  virtual llvm::Value *plus() = 0;

  virtual llvm::Value *eq(Value &rv) = 0;
  virtual llvm::Value *ne(Value &rv) = 0;
  virtual llvm::Value *lt(Value &rv) = 0;
  virtual llvm::Value *le(Value &rv) = 0;
  virtual llvm::Value *gt(Value &rv) = 0;
  virtual llvm::Value *ge(Value &rv) = 0;
};

} // namespace Compiler
