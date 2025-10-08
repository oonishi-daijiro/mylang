#pragma once

#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
namespace Compiler {
class Scope;

class Symbol {
  std::string sname;
  size_t hash;
  Scope *s{nullptr};

public:
  Symbol(const std::string &name);
  virtual ~Symbol() = default;

  const std::string name() const;
  virtual const std::string kind() const = 0;

  virtual bool operator==(const std::string &name) final;
  virtual bool operator!=(const std::string &name) final;

  virtual bool operator==(const Symbol &r) final;
  virtual bool operator!=(const Symbol &r) final;

  template <typename... T>
  bool isa()
    requires(std::is_base_of_v<Symbol, T> && ...)
  {
    return ((dynamic_cast<T *>(this) != nullptr) || ...);
  }

  template <typename T> T *expect() {
    if (!isa<T>()) {
      throw std::runtime_error(
          std::format("expected type {} but {}", typeid(T).name(), kind()));
    }
    return dynamic_cast<T *>(this);
  }

  template <typename T> T *cast() { return dynamic_cast<T *>(this); }

  virtual void setScope(Scope &) final;
  virtual void registerToCurrentScope() final;
  virtual Scope &currentScope() const final;
};

} // namespace Compiler
