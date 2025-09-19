#pragma once

#include <llvm/IR/Type.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

#include "ast.hpp"
#include "kind.hpp"
#include "utils.hpp"

namespace Compiler {

class TypeTrait : public LLVMBuilder {

public:
  virtual ~TypeTrait() = default;
  virtual std::string name() = 0;

  template <util::NOPTR_NOREF T>
  T *except()
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
};

class Type final {
  llvm::Type *inst{nullptr};
  std::string tname{"unresolved_type"};
  TypeTrait *tr{nullptr};
  Kind *k{nullptr};

  static inline std::map<std::string, Type> typeset{};
  static inline std::vector<std::shared_ptr<TypeTrait>> traitset{};

public:
  static const Type &GetType(const std::string &name) {
    if (name == "unresolved_type") {
      throw std::runtime_error("cannot get type of unresolved type");
    }

    if (typeset.count(name)) {
      return typeset.at(name);
    } else {
      throw std::runtime_error(
          std::format("type {} is no defined as builtin type", name));
    }
  }
  template <typename T>
  static inline void DefineNewType(const std::string &name, llvm::Type *inst)
    requires(std::is_base_of_v<TypeTrait, T>)
  {
    if (typeset.count(name)) {
      throw std::runtime_error(std::format("type {} is already defined", name));
    } else {
      auto trait = new T{};
      traitset.emplace_back(trait);

      typeset.emplace(
          std::piecewise_construct, std::forward_as_tuple(name),
          std::forward_as_tuple(std::move(Type{name, inst, trait})));
    }
  }

  static inline void RegisterType(const Type &t) {
    if (typeset.count(t.tname)) {
      throw std::runtime_error(
          std::format("type {} is already defined", t.tname));
    } else if (t.tname == "unresolved_type") {
      throw std::runtime_error("cannot register unresolved type");
    } else {
      typeset.emplace(std::piecewise_construct, std::forward_as_tuple(t.tname),
                      std::forward_as_tuple(t.tname, t.inst, t.tr));
    }
  }

  Type(const Type &) = default;
  Type(Type &&) = default;
  Type() {};

  Type(const std::string &name, llvm::Type *inst, TypeTrait *tr)
      : inst{inst}, tname{name}, tr{tr} {}

  Type(const std::string &name) {
    auto &&t = GetType(name);
    *this = t;
  }

  Type &operator=(const std::string &name) {
    *this = GetType(name);
    return *this;
  }

  bool operator==(const Type &r) const { return this->tname == r.tname; };
  bool operator!=(const Type &r) const { return !(*this == r); }
  const std::string &name() const { return tname; }

  Type &operator=(const Type &r) {
    if (this != &r) {
      this->inst = r.inst;
      this->tname = r.tname;
      this->tr = r.tr;
    }
    return *this;
  };

  TypeTrait *trait() const { return tr; }
  llvm::Type *getTypeInst() const {
    if (tname == "unresolved_type") {
      throw std::runtime_error("cannot resolve type");
    }
    return inst;
  }
};

} // namespace Compiler
