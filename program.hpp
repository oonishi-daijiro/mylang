#pragma once

#include "ast.hpp"
#include "function.hpp"
#include <string>
namespace Compiler {

class Program : public Node, public ScopeSemantic {
  Function &f;

public:
  Program(Function &func) : Node{&func}, f{func} {}
  virtual void gen() override { f.gen(); }
  virtual std::string to_string() override { return std::format("|Program|"); }
  virtual void resolveScope() override {
    walkAllChildlenBF(defaultScopeInitalizer);
  }
};

} // namespace Compiler
