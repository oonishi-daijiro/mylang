#pragma once

#include "type.hpp"
#include "type_traits.hpp"
#include <llvm/IR/DerivedTypes.h>

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

  template <typename T>
  bool isa()
    requires(std::is_base_of_v<Kind, T>)
  {
    return dynamic_cast<T *>(this) != nullptr;
  }

  template <typename T> T *cast() { return dynamic_cast<T *>(this); }

  virtual std::string name() = 0;
  template <typename T> static inline Kind *New(auto &&...arg) {
    auto k = new T{std::forward<decltype(arg)>(arg)...};
    kindset.emplace_back(k);
    return k;
  }
};

class PrimitiveKind : public Kind {
public:
  virtual std::string name() override { return "primitive"; }
  static inline Type Apply(const Type &t) {
    auto k = Kind::New<PrimitiveKind>();
    return {t.name(), t.getTypeInst(), t.trait(), k};
  }
};

class ArrayKind : public Kind {
  Type elementTy;
  size_t arraySize;

public:
  virtual ~ArrayKind() = default;
  virtual std::string name() { return "array"; }

  ArrayKind(const Type &elmTy, size_t s) : elementTy{elmTy}, arraySize{s} {}

  const Type &element() { return elementTy; }
  const size_t size() { return arraySize; }
  static inline Type Apply(const Type &t, size_t size) {
    auto arrayHeadPtrInst = llvm::PointerType::get(t.getTypeInst(), 0);
    auto tyName = std::format("array[{}]", size);
    auto tr = TypeTrait::New<ArrayTyTrait>();
    auto k = Kind::New<ArrayKind>(t, size);
    return {tyName, arrayHeadPtrInst, tr, k};
  }
};

class PointerKind : public Kind {};

} // namespace Compiler
