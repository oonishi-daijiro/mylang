#pragma once

#include "ast.hpp"
#include "statements.hpp"

namespace Compiler {

class Function : public Node {
  Block &body;

public:
  Function(Block *body) : body{*body}, Node{body} {}
  virtual std::string to_string() override;
  virtual void gen() override { body.gen(); }
};
} // namespace Compiler
