#include "program.hpp"
#include "function.hpp"
namespace Compiler {
Program::Program(std::vector<Function *> &&funcs)
    : functions{std::move(funcs)} {
  for (auto func : functions) {
    appendChild(func);
  }
}

void Program::gen() {
  for (auto &&func : functions) {
    func->gen();
  }
}
std::string Program::to_string() { return std::format("|Program|"); }
void Program::resolveScope() { walkAllChildlenBF(defaultScopeInitalizer); }
} // namespace Compiler
