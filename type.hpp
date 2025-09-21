#pragma once

#include <cstddef>
#include <functional>
#include <llvm/IR/Type.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

#include "ast.hpp"
#include "utils.hpp"

namespace Compiler {
class Kind;

class TypeTrait : public LLVMBuilder {
  static inline std::map<std::string, std::shared_ptr<TypeTrait>> traitset;

public:
  TypeTrait() = default;
  virtual ~TypeTrait() = default;
  virtual std::string name() = 0;

  template <util::NOPTR_NOREF T>
  T *expect()
    requires(std::is_base_of_v<TypeTrait, T>)
  {
    auto ptr = dynamic_cast<T *>(this);
    if (ptr == nullptr) {
      throw std::runtime_error(std::format(
          "type trait \"{}\" is not satisfies trait {}", name(), T::name));
    } else {
      return ptr;
    }
  }

  template <typename T>
  static inline TypeTrait *New()
    requires(std::is_base_of_v<TypeTrait, T>)
  {
    auto tr = new T{};
    if (!traitset.contains(tr->name())) {
      traitset.emplace(std::piecewise_construct,
                       std::forward_as_tuple(tr->name()),
                       std::forward_as_tuple(tr));
      return tr;
    } else {
      delete tr;
      return nullptr;
    }
  }
};

class Type final {
  static inline size_t unresolved_type_hash =
      std::hash<std::string>{}(std::string{"unresolved_type"});

  llvm::Type *inst{nullptr};
  std::string tname{"unresolved_type"};
  size_t hash{std::hash<std::string>()(tname)};
  TypeTrait *tr{nullptr};
  Kind *k{nullptr};

  static inline std::map<std::string, Type> typeset{};

  friend class Kind;
  void setKind(Kind *kind);

public:
  static const Type &GetType(const std::string &name);

  static void DefineNewPrimitiveType(const std::string &name, llvm::Type *inst,
                                     TypeTrait *tr);

  static inline void RegisterType(const Type &t);

  Type(const std::string &name, llvm::Type *inst, TypeTrait *tr, Kind *k);
  Type(const std::string &name);
  Type(const Type &) = default;
  Type(Type &&) = default;
  ~Type() = default;
  Type();

  Type &operator=(const std::string &name);
  Type &operator=(const Type &r);

  bool operator==(const std::string &) const;
  bool operator==(const Type &r) const;

  bool operator!=(const std::string &) const;
  bool operator!=(const Type &r) const;

  const std::string &name() const;
  TypeTrait *trait() const;
  llvm::Type *getTypeInst() const;
  Kind *kind();
};

} // namespace Compiler
