#pragma once

#include <optional>
#include <set>
#include <string>

#include "symbol.hpp"

namespace Compiler {
class Scope {
  Scope *parent{nullptr};

  static inline std::set<std::string> scopeNameSet{};

  std::set<Symbol *> symbolset{};
  std::string scopeName;

protected:
  friend class Symbol;

public:
  Scope(const std::string &name = "");

  bool existsOnSameScope(const std::string &name);
  const std::set<Symbol *> &symbols() const;
  bool available(const std::string &name);
  Symbol *find(const std::string &name);
  void registerSymbol(Symbol *s);
  const std::string &name();
  void setParent(Scope &p);
  Scope &getParent();
};

} // namespace Compiler
