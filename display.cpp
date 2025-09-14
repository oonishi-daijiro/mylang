#include <format>
#include <string>

#include "ast.hpp"
#include "expressions.hpp"
#include "operators.hpp"
#include "statements.hpp"

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
// std::string BinaryOperator::to_string() {
//   return std::format("[BinaryOperator {}]", kind());
// }

// std::string ConstantVariable::to_string() {
//   return std::format("[ConstantVariable \"{}\"]", name);
// }

// std::string OrderedUnaryOperator::to_string() {
//   return std::format("[UnaryOperator {}]", kind());
// }

// std::string ImplicitBooleanCast::to_string() {
//   return std::format("[ImplicitBooleanCast:\"{}\"]", value.to_string());
// }

// statements : enclose <>
std::string MutableVarDeclaration::to_string() {
  return std::format("<MutableVarDecl \"{}\">", var->getname());
}

std::string Assign::to_string() {
  return std::format("<Assign: {} = {}>", lv.to_string(), rv.to_string());
}

std::string CompoundStatement::to_string() {
  return std::format("(CompoundStatement)");
}
// std::string ConstantVarDecl::to_string() {
//   return std::format("<ImutableVarDecl {}>", var.name);
// }

std::string Ret::to_string() { return std::format("<Return>"); }
std::string IfStatement::to_string() { return "<If>"; }
// std::string ElseStatement::to_string() { return "<Else>"; }

} // namespace Compiler
