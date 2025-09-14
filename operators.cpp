
#include "operators.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "traits.hpp"

#include <functional>
#include <initializer_list>
#include <llvm/IR/Operator.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace Compiler {
std::string BooleanOperator::operationRetTypeName() { return "boolean"; }

std::string AddOperator::kind() { return "+"; }
std::string SubOperator::kind() { return "-"; }
std::string MulOperator::kind() { return "*"; }
std::string DivOperator::kind() { return "/"; }
std::string EqOperator::kind() { return "=="; }
std::string NeqOperator::kind() { return "!="; }
std::string LtOperator::kind() { return "<"; }
std::string LeOperator::kind() { return "<="; }
std::string GtOperator::kind() { return ">"; }
std::string GeOperator::kind() { return ">="; }
std::string MinusOperator::kind() { return "(-)"; }

llvm::Value *AddOperator::get() {
  auto trait = type.trait()->except<Field>();
  return trait->add(lv, rv);
}

llvm::Value *SubOperator::get() {
  auto trait = type.trait()->except<Field>();
  return trait->sub(lv, rv);
}

llvm::Value *MulOperator::get() {
  auto trait = type.trait()->except<Field>();
  return trait->mul(lv, rv);
}

llvm::Value *DivOperator::get() {
  auto trait = type.trait()->except<Field>();
  return trait->div(lv, rv);
}

llvm::Value *MinusOperator::get() {
  auto trait = type.trait()->except<Ordered>();
  return trait->minus(o);
}

llvm::Value *EqOperator::get() {
  auto trait = lv.type.trait()->except<Boolean>();
  return trait->eq(lv, rv);
}

llvm::Value *NeqOperator::get() {
  auto trait = lv.type.trait()->except<Boolean>();
  return trait->ne(lv, rv);
}

llvm::Value *LtOperator::get() {
  auto trait = lv.type.trait()->except<Ordered>();
  return trait->lt(lv, rv);
}

llvm::Value *LeOperator::get() {
  auto trait = lv.type.trait()->except<Ordered>();
  return trait->le(lv, rv);
}

llvm::Value *GtOperator::get() {
  auto trait = lv.type.trait()->except<Ordered>();
  return trait->gt(lv, rv);
}

llvm::Value *GeOperator::get() {
  auto trait = lv.type.trait()->except<Ordered>();
  return trait->ge(lv, rv);
}

}; // namespace Compiler
