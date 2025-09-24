#pragma once

#include "ast.hpp"
#include "function.hpp"
#include <string>
#include <vector>
namespace Compiler {

class Program : public Node, public ScopeSemantic {
  std::vector<Function *> functions;

public:
  Program(std::vector<Function *> &&);
  virtual void gen() override;
  virtual std::string to_string() override;
  virtual void resolveScope() override;
};

} // namespace Compiler
