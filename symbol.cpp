#include "symbol.hpp"
#include "scope.hpp"
#include "utils.hpp"

namespace Compiler {
void Symbol::setScope(Scope &scope) { s = &scope; }
Scope &Symbol::currentScope() { return *s; }
void Symbol::registerToCurrentScope() { currentScope().registerSymbol(this); }

Symbol::Symbol(const std::string &name)
    : sname{name}, hash{std::hash<std::string>{}(name)} {}

const std::string Symbol::name() const { return sname; }

bool Symbol::operator==(const Symbol &r) { return this->hash == r.hash; }
bool Symbol::operator!=(const Symbol &r) { return this->hash != r.hash; }

bool Symbol::operator==(const std::string &name) {
  return hash == std::hash<std::string>{}(name);
}

bool Symbol::operator!=(const std::string &name) {
  return hash != std::hash<std::string>{}(name);
}

} // namespace Compiler
