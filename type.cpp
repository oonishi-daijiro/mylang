#include "type.hpp"
#include "errors.hpp"
#include "kind.hpp"
#include <functional>
#include <stdexcept>

namespace Compiler {

const Type &Type::GetType(const std::string &name) {
  if (name == "unresolved_type") {
    throw std::runtime_error("cannot get type of unresolved type");
  }

  if (typeset.count(name)) {
    return typeset.at(name);
  } else {
    throw std::runtime_error(
        std::format("type {} is no defined as builtin type", name));
  }
}

void Type::RegisterType(const Type &t) {
  if (typeset.count(t.tname)) {
    throw std::runtime_error(
        std::format("type {} is already defined", t.tname));
  } else if (t.tname == "unresolved_type") {
    throw std::runtime_error("cannot register unresolved type");
  } else {
    typeset.emplace(std::piecewise_construct, std::forward_as_tuple(t.tname),
                    std::forward_as_tuple(t.tname, t.inst, t.tr, t.k));
  }
}
void Type::DefineNewPrimitiveType(const std::string &name, llvm::Type *inst,
                                  TypeTrait *tr) {
  if (typeset.count(name)) {
    throw std::runtime_error(std::format("type {} is already defined", name));
  } else {
    auto kind = Kind::New<PrimitiveKind>();
    typeset.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                    std::forward_as_tuple(name, inst, tr, kind));
  }
}

Type::Type() {};

Type::Type(const std::string &name, llvm::Type *inst, TypeTrait *tr, Kind *k)
    : inst(inst), tname(name), tr(tr), k(k),
      hash{std::hash<std::string>()(name)} {}

Type::Type(const std::string &name) {
  auto &&t = GetType(name);
  *this = t;
}

Type &Type::operator=(const std::string &name) {
  *this = GetType(name);
  return *this;
}

bool Type::operator==(const Type &r) const { return this->hash == r.hash; };
bool Type::operator!=(const Type &r) const { return !(*this == r); }
bool Type::operator==(const std::string &name) const {
  return *this == GetType(name);
}
bool Type::operator!=(const std::string &name) const {
  return !(*this == GetType(name));
}
const std::string &Type::name() const { return tname; }

Type &Type::operator=(const Type &r) {
  if (this != &r) {
    this->inst = r.inst;
    this->tname = r.tname;
    this->tr = r.tr;
    this->k = r.k;
    this->hash = r.hash;
  }
  return *this;
};

TypeTrait *Type::trait() const { return tr; }
Kind *Type::kind() { return k; }
void Type::setKind(Kind *kind) { k = kind; }

llvm::Type *Type::getTypeInst() const {
  if (hash == unresolved_type_hash) {
    throw std::runtime_error("cannot resolve type");
  }
  return inst;
}

bool operator==(const std::vector<const Type *> &lv,
                const std::vector<const Type *> &rv) {
  if (lv.size() != rv.size()) {
    return false;
  }
  bool same = true;
  for (int i = 0; i < lv.size(); i++) {
    same &= (*lv[i] == *rv[i]);
  }
  return same;
}
} // namespace Compiler
