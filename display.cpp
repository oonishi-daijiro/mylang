#include <format>
#include <string>

#include "ast.hpp"
#include "block.hpp"
#include "control_statements.hpp"
#include "expressions.hpp"
#include "function.hpp"
#include "operators.hpp"
#include "statement.hpp"

namespace Compiler {

// function:
std::string Function::to_string() {
  return std::format("function {}{}", name, sig.to_string());
}

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

std::string ArrayExpr::to_string() {
  return std::format("[Array: {}:[{}]]", type.name(), valstr.str());
}

std::string StringExpr::to_string() {
  return std::format("[String: \"{}\"]", value);
}

std::string LocalVariable::to_string() {
  return std::format("[LocalVariable:\"{}\" : {}]", name, type.name());
}

std::string Operator::to_string() {
  return std::format("[Operator {} : {}]", kind(), type.name());
}

// statements : enclose <>
std::string MutableLocalVarDeclaration::to_string() {
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
