#pragma once

#include <format>
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
  Symbol(const std::string &name)
      : sname{name}, hash{std::hash<std::string>{}(name)} {}
  virtual ~Symbol() = default;

  const std::string name() const { return sname; }
  virtual const std::string kind() const = 0;

  virtual bool operator==(const Symbol &r) final {
    return this->hash == r.hash;
  }
  virtual bool operator!=(const Symbol &r) final {
    return this->hash != r.hash;
  }
  virtual bool operator==(const std::string &name) final {
    return hash == std::hash<std::string>{}(name);
  }

  virtual bool operator!=(const std::string &name) final {
    return hash != std::hash<std::string>{}(name);
  }

  template <typename T>
  bool isa()
    requires(std::is_base_of_v<Symbol, T>)
  {
    return dynamic_cast<T *>(this) != nullptr;
  }
  template <typename T> T *expect() {
    if (isa<T>()) {
      throw std::runtime_error(
          std::format("expected type {} but not", typeid(T).name()));
    }
    return dynamic_cast<T *>(this);
  }

  template <typename T> T *cast() { return dynamic_cast<T *>(this); }

  virtual void setScope(Scope &) final;
  virtual Scope &scope() final;
};

} // namespace Compiler
