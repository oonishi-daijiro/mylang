#include <format>
#include <string>

#include "ast.hpp"
#include "control_statements.hpp"
#include "expressions.hpp"
#include "operators.hpp"
#include "statement.hpp"

namespace Compiler {
// block  : enclose {}
std::string Block::to_string() { return "{Block}"; }

// expressions : enclose []
std::string DoubleExpr::to_string() {
  return std::format("[{} : {}]", type.name(), value);
}

std::string IntegerExpr::to_string() {
  return std::format("[{} : {}]", type.name(), value);
}

std::string BooleanExpr::to_string() {
  return std::format("[{} : {}]", type.name(), (value ? "true" : "false"));
}

std::string Variable::to_string() {
  return std::format("[MutableVariable:\"{}\" : {}]", name, type.name());
}

std::string Operator::to_string() {
  return std::format("[Operator {} : {}]", kind(), type.name());
}

// statements : enclose <>
std::string MutableVarDeclaration::to_string() {
  return std::format("<MutableVarDecl \"{}\">", var->getname());
}

std::string Assign::to_string() {
  return std::format("<Assign: {} = {}>", lv.to_string(), rv.to_string());
}

std::string CompoundStatement::to_string() {
  return std::format("<CompoundStatement>");
}

std::string Ret::to_string() { return std::format("<Return>"); }
std::string IfStatement::to_string() { return "<If>"; }
std::string ForStatement::to_string() { return "<For>"; }
std::string ContinueStatement::to_string() { return "<Continue>"; }
std::string BreakStatement::to_string() { return "<Break>"; }
std::string WhileStatement::to_string() { return "<While>"; }

} // namespace Compiler
