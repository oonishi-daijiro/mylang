#include "symbol.hpp"
#include "scope.hpp"
#include "utils.hpp"

namespace Compiler {
void Symbol::setScope(Scope &scope) { s = &scope; }
Scope &Symbol::currentScope() const { return *s; }
void Symbol::registerToCurrentScope() { currentScope().registerSymbol(this); }

Symbol::Symbol(const std::string &name)
    : sname{name}, hash{std::hash<std::string>{}(name)} {}

const std::string Symbol::name() const { return sname; }

bool Symbol::operator==(const Symbol &r) {
  auto sameName = this->hash == r.hash;
  auto sameScope = &this->currentScope() == &r.currentScope();
  return sameName && sameScope;
}

bool Symbol::operator!=(const Symbol &r) { return !(*this == r); }

bool Symbol::operator==(const std::string &name) {
  return hash == std::hash<std::string>{}(name);
}

bool Symbol::operator!=(const std::string &name) {
  return hash != std::hash<std::string>{}(name);
}

} // namespace Compiler
