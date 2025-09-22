#include "scope.hpp"
#include <vector>

namespace Compiler {
Scope::Scope(const std::string &name) : scopeName{name} {
  if (scopeNameSet.contains(name)) {
    scopeName += std::to_string(scopeNameSet.size());
  }
  scopeNameSet.emplace(scopeName);
}

void Scope::setParent(Scope &p) {
  if (&p != this) {
    parent = &p;
  }
}

const std::string &Scope::name() { return scopeName; }

Symbol *Scope::find(const std::string &name) {
  std::vector<Scope *> availableScope{};

  Scope *s = this;
  while (s != nullptr) {
    availableScope.emplace_back(s);
    s = s->parent;
  }

  Symbol *target = nullptr;
  for (auto &&scope : availableScope) {
    for (auto &&symbol : scope->symbolset) {
      if ((*symbol) == name) {
        target = symbol;
        break;
      }
    }
    if (target != nullptr) {
      break;
    }
  }

  return target;
}

bool Scope::available(const std::string &name) { return find(name) != nullptr; }

bool Scope::existsOnSameScope(const std::string &name) {
  bool exists = false;
  for (auto &&symbol : symbolset) {
    if (symbol->name() == name) {
      exists = true;
    }
  }
  return exists;
}

void Scope::registerSymbol(Symbol *s) {
  if (s != nullptr) {
    symbolset.emplace(s);
  }
}

const std::set<Symbol *> &Scope::symbols() const { return symbolset; }
Scope &Scope::getParent() { return *parent; }
} // namespace Compiler
