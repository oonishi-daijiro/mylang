#pragma once

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "symbol.hpp"

namespace Compiler {
class Scope {
  Scope *parent{nullptr};
  std::set<Symbol *> symbolset{};

protected:
  friend class Symbol;

public:
  Scope() {}

  void setParent(Scope &p) { parent = &p; }

  Symbol *find(const std::string &name) {
    std::vector<Scope *> availableScope{};

    Scope *s = this;
    while (s != nullptr) {
      availableScope.emplace_back(s);
      s = s->parent;
    }

    Symbol *target = nullptr;
    for (auto &&scope : availableScope) {
      for (auto &&symbol : scope->symbolset) {
        if (*symbol == name) {
          target = symbol;
        }
      }
    }
    return target;
  }

  bool exists(const std::string &name) { return find(name) != nullptr; }

  bool existsOnSameScope(const std::string &name) {
    bool exists = false;
    for (auto &&symbol : symbolset) {
      if (symbol->name() == name) {
        exists = true;
      }
    }
    std::cout<< "ss" <<std::endl;
    return exists;
  }

  void registerSymbol(Symbol *s) {
    if (s != nullptr) {
      symbolset.emplace(s);
    }
  }
};

} // namespace Compiler
