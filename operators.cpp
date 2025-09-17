
#include "operators.hpp"
#include "traits.hpp"
#include <llvm/IR/Value.h>

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
std::string IncrementOperator::kind() { return "(...)++"; }
std::string DecrementOperator::kind() { return "(...)--"; }

MagmaOperator::MagmaOperator(Expression *lv, Expression *rv)
    : Operator{lv, rv}, lv{*lv}, rv{*rv} {}

void MagmaOperator::resolveType() {
  if (lv.type != rv.type) {
    throw TypeError(this->info, std::format("type mismatch: '{}' vs '{}'",
                                            lv.type.name(), rv.type.name()));
  } else {
    type.resolve(lv.type.name());
  }
};

BinaryOperator::BinaryOperator(Expression *lv, Expression *rv)
    : Operator{lv, rv}, lv{*lv}, rv{*rv} {}

void BinaryOperator::resolveType() {
  if (lv.type != rv.type) {
    throw TypeError(this->info, std::format("type mismatch: '{}' vs '{}'",
                                            lv.type.name(), rv.type.name()));
  } else {
    type.resolve(operationRetTypeName());
  }
}

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

llvm::Value *IncrementOperator::get() {
  auto trait = o->type.trait()->except<Field>();
  auto &unit = trait->unit();
  auto prev = o->get();
  builder->CreateStore(trait->add(*o, unit), o->ptr());
  return prev;
}

llvm::Value *DecrementOperator::get() {
  auto trait = o->type.trait()->except<Field>();
  auto &unit = trait->unit();
  auto prev = o->get();
  builder->CreateStore(trait->sub(*o, unit), o->ptr());
  return prev;
}
}; // namespace Compiler
