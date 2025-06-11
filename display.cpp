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
std::string Double::to_string() { return std::format("[Numeric:{}]", value); }
std::string Integer::to_string() { return std::format("[Integer:{}]", value); }

// std::string BinaryOperator::to_string() {
//   return std::format("[BinaryOperator {}]", kind());
// }
// std::string ConstantVariable::to_string() {
//   return std::format("[ConstantVariable \"{}\"]", name);
// }
// std::string MutableVariable::to_string() {
//   return std::format("[MutableVariable \"{}\"]", name);
// }
// std::string OrderedUnaryOperator::to_string() {
//   return std::format("[UnaryOperator {}]", kind());
// }
// std::string VariableReferece::to_string() {
//   return std::format("[Reference {}]", s.name);
// }
// std::string ImplicitBooleanCast::to_string() {
//   return std::format("[ImplicitBooleanCast:\"{}\"]", value.to_string());
// }

// // statements : enclose <>
// std::string MutableVarDecl::to_string() {
//   return std::format("<MutableVarDecl \"{}\">", var.name);
// }
// std::string ConstantVarDecl::to_string() {
//   return std::format("<ImutableVarDecl {}>", var.name);
// }

std::string Ret::to_string() { return std::format("<Return>"); }
std::string IfStatement::to_string() { return "<If>"; }
std::string ElseStatement::to_string() { return "<Else>"; }

// kind string of operator
// std::string AddOperator::kind() { return "+"; }
// std::string SubOperator::kind() { return "-"; }
// std::string MulOperator::kind() { return "*"; }
// std::string DivOperator::kind() { return "/"; }
// std::string EqualOperator::kind() { return "=="; }
// std::string NonEqualOperator::kind() { return "!="; };
// std::string LessThanOperator::kind() { return "<"; }
// std::string GreaterThanOperator::kind() { return ">"; }
// std::string LessThanOrEqOperator::kind() { return "<="; }
// std::string PlusOperator::kind() { return "(+)"; }
// std::string GreaterThanOrEqOperator::kind() { return ">="; }
// std::string MinusOperator::kind() { return "(-)"; }
// std::string AssignOperator::kind() { return "="; }
} // namespace AST
