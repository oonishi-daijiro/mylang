#pragma once

#include <llvm/IR/Value.h>
#include <optional>
#include <variant>

#include "ast.hpp"
#include "type.hpp"
#include "utils.hpp"

namespace Compiler {

class Immutable final {
  Immutable() = delete;
};

class Mutable final {
  Mutable() = delete;
};

template <typename T> class Substance;

class Value {
public:
  virtual ~Value() = default;
  virtual llvm::Value *get() = 0;

  Value() = default;
  Type type{};

  template <typename T> bool hasSubstanceOf() {
    return util::isa<Substance<T>>(this);
  }

  template <typename T> std::optional<Substance<T> *> getSubstanceOf() {
    return util::safe_cast<Substance<T>>(this);
  }

  std::variant<Value *, Substance<Mutable> *, Substance<Immutable> *>
  getSubstance() {
    if (hasSubstanceOf<Mutable>()) {
      return {util::cast<Substance<Mutable>>(this)};
    } else if (hasSubstanceOf<Immutable>()) {
      return {util::cast<Substance<Immutable>>(this)};
    } else {
      return {this};
    }
  }
};

template <> class Substance<Mutable> {
public:
  virtual ~Substance() = default;

  virtual void set(Value &) = 0;
  virtual llvm::Value *ptr() = 0;
};

template <> class Substance<Immutable> {
public:
  virtual ~Substance() = default;
  virtual llvm::Value *ptr() = 0;
};

class ConstantValue final : public Value {
  llvm::Value *val{nullptr};

public:
  ConstantValue(llvm::Value *val, const std::string &tyName);
  virtual ~ConstantValue() = default;
  virtual llvm::Value *get() final override;
};

} // namespace Compiler
