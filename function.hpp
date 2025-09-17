#pragma once

#include "ast.hpp"

namespace Compiler {

class Function : public Node {
  Block &body;

public:
  Function(Block *body) : Node{body}, body{*body} {}
  virtual std::string to_string() override;
  virtual void gen() override { body.gen(); }
};
} // namespace Compiler
