#include "symbol.hpp"
#include "scope.hpp"

namespace Compiler {
void Symbol::setScope(Scope &scope) { s = &scope; }
Scope &Symbol::scope() { return *s; }

} // namespace Compiler
