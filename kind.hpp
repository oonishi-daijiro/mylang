#pragma once

#include "function.hpp"
#include "type.hpp"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>

namespace Compiler {

class Kind {
  struct kind_deleter {
    ~kind_deleter() {
      for (auto &&k : kindset)
        delete k;
    }
  };

  static inline std::vector<Kind *> kindset{};
  static inline kind_deleter del{};

protected:
  Kind() = default;

public:
  virtual ~Kind() = default;

  virtual std::string name() = 0;

  template <typename T>
  bool isa()
    requires(std::is_base_of_v<Kind, T>)
  {
    return dynamic_cast<T *>(this) != nullptr;
  }

  template <typename T> T *cast() { return dynamic_cast<T *>(this); }

  template <typename T> static inline Kind *New(auto &&...arg) {
    auto k = new T{std::forward<decltype(arg)>(arg)...};
    kindset.emplace_back(k);
    return k;
  }
};

class PrimitiveKind : public Kind {
public:
  virtual std::string name() override;

  static inline Type Apply(const Type &t);
};

class ArrayKind : public Kind {
  Type elementTy;
  size_t arraySize;

public:
  virtual ~ArrayKind() = default;
  ArrayKind(const Type &elmTy, size_t s);
  virtual std::string name();

  static Type Apply(const Type &t, size_t size);

  const Type &element();
  const size_t size();
};

class StringKind : public ArrayKind {
public:
  StringKind(size_t s);
  static Type Apply(size_t size);
};

class FunctionSignature;

class FunctionKind : public Kind {
  const FunctionSignature &sig;
  llvm::FunctionType *funcTy;

public:
  FunctionKind(const FunctionSignature &, llvm::FunctionType *);
  virtual std::string name() override;
  static Type Apply(const FunctionSignature &sig);
  llvm::FunctionType *funcType();
  const FunctionSignature &signature() const;
};

class PointerKind : public Kind {};

} // namespace Compiler
