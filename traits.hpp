#pragma once

#include "ast.hpp"
#include <format>
#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <map>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace Compiler {

template <typename T>
concept NOPTR_NOREF = requires() {
  !std::is_pointer_v<T>;
  !std::is_reference_v<T>;
};

class TypeTrait : public LLVMBuilder {

public:
  virtual ~TypeTrait() = default;
  virtual std::string name() = 0;

  template <NOPTR_NOREF T>
  T *except()
    requires(std::is_base_of_v<TypeTrait, T>)
  {
    auto ptr = dynamic_cast<T *>(this);
    if (ptr == nullptr) {
      throw std::runtime_error(std::format(
          "type trait \"{}\" is not satisfies trait {}", name(), T::name));
    } else {
      return ptr;
    }
  }
};

class AnyTrait : public TypeTrait {
  virtual std::string name() { return "any"; }
};

class Type {
  llvm::Type *inst{nullptr};
  std::string tyName{"unresolved_type"};
  TypeTrait *tr{nullptr};

  struct deleter {
    ~deleter() {
      for (auto &&[_, t] : typeset) {
        auto &&[__, trait] = t;
        delete trait;
      }
    }
  };

  static inline deleter d{};
  static inline std::map<std::string, std::tuple<llvm::Type *, TypeTrait *>>
      typeset{{"unresolved_type", {nullptr, nullptr}}};

public:
  static void DefineNewType(const std::string &name, llvm::Type *inst,
                            TypeTrait *trait = new AnyTrait{}) {
    if (typeset.count(name)) {
      throw std::runtime_error(
          std::format("type \"{}\" is already defined", name));
    } else {
      typeset.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                      std::forward_as_tuple(inst, trait));
    }
  }

  Type(Type &&src) = default;
  Type(const Type &cp) = default;
  Type() {}

  const std::string &name() const { return tyName; }
  llvm::Type *getTypeInst() const {
    if (tyName == "unresolved_type") {
      throw std::format("this type is not resolved");
    } else {
      return inst;
    }
  }

  void resolve(const std::string &name) {
    if (typeset.count(name)) {
      auto &&[inst, trait] = typeset.at(name);
      this->inst = inst;
      this->tr = trait;
      this->tyName = name;
    } else {
      throw std::format("type \"{}\" is not defined", name);
    }
  }

  void resolve(const Type &ty) { resolve(ty.name()); }

  bool operator==(const Type &rv) const { return tyName == rv.tyName; }
  bool operator!=(const Type &rv) const { return !(*this == rv); }
  Type &operator=(const Type &rv) {
    this->inst = rv.inst;
    this->tyName = rv.tyName;
    this->tr = rv.tr;
    return *this;
  }
  TypeTrait *trait() { return tr; };
};

class Value {
public:
  virtual ~Value() = default;
  virtual llvm::Value *get() = 0;

  Value() {}
  Type type{};
};

class ConstantValue final : public Value {
  llvm::Value *val{nullptr};

public:
  ConstantValue(llvm::Value *val, const std::string &tyName) : val{val} {
    type.resolve(tyName);
  }
  virtual ~ConstantValue() = default;
  virtual llvm::Value *get() final override { return val; };
};

class Boolean : virtual public TypeTrait {
public:
  static inline std::string name{"Boolean"};
  virtual ~Boolean() = default;

  virtual llvm::Value *eq(Value &lv, Value &rv) = 0;
  virtual llvm::Value *ne(Value &lv, Value &rv) = 0;
};

class Field : virtual public TypeTrait {

public:
  static inline std::string name{"Field"};
  virtual ~Field() = default;
  virtual Value &unit() = 0;

  virtual llvm::Value *add(Value &lv, Value &rv) = 0;
  virtual llvm::Value *sub(Value &lv, Value &rv) = 0;
  virtual llvm::Value *mul(Value &lv, Value &rv) = 0;
  virtual llvm::Value *div(Value &lv, Value &rv) = 0;
};

class Ordered : virtual public Field, virtual public Boolean {
public:
  static inline std::string name{"Ordered"};
  virtual ~Ordered() = default;

  virtual llvm::Value *minus(Value &v) = 0;
  virtual llvm::Value *plus(Value &v) = 0;

  virtual llvm::Value *lt(Value &lv, Value &rv) = 0;
  virtual llvm::Value *le(Value &lv, Value &rv) = 0;
  virtual llvm::Value *gt(Value &lv, Value &rv) = 0;
  virtual llvm::Value *ge(Value &lv, Value &rv) = 0;
};

// implmentation of builtin types

class DoubleTyTrait : virtual public Ordered {
  Value *unitVal{nullptr};

public:
  virtual std::string name() override { return "DoubleTyTrait"; }
  virtual ~DoubleTyTrait() {
    if (unitVal != nullptr) {
      delete unitVal;
    }
  };

  virtual Value &unit() override;

  virtual llvm::Value *add(Value &lv, Value &rv) override;
  virtual llvm::Value *sub(Value &lv, Value &rv) override;
  virtual llvm::Value *mul(Value &lv, Value &rv) override;
  virtual llvm::Value *div(Value &lv, Value &rv) override;

  virtual llvm::Value *minus(Value &v) override;
  virtual llvm::Value *plus(Value &v) override;

  virtual llvm::Value *eq(Value &lv, Value &rv) override;
  virtual llvm::Value *ne(Value &lv, Value &rv) override;

  virtual llvm::Value *lt(Value &lv, Value &rv) override;
  virtual llvm::Value *le(Value &lv, Value &rv) override;
  virtual llvm::Value *gt(Value &lv, Value &rv) override;
  virtual llvm::Value *ge(Value &lv, Value &rv) override;
};

class IntegerTyTrait : virtual public Ordered {
  Value *unitVal{nullptr};

public:
  virtual std::string name() override { return "IntegerTyTrait"; }
  virtual ~IntegerTyTrait() {
    if (unitVal != nullptr) {
      delete unitVal;
    }
  };

  virtual Value &unit() override;

  virtual llvm::Value *add(Value &lv, Value &rv) override;
  virtual llvm::Value *sub(Value &lv, Value &rv) override;
  virtual llvm::Value *mul(Value &lv, Value &rv) override;
  virtual llvm::Value *div(Value &lv, Value &rv) override;

  virtual llvm::Value *minus(Value &v) override;
  virtual llvm::Value *plus(Value &v) override;

  virtual llvm::Value *eq(Value &lv, Value &rv) override;
  virtual llvm::Value *ne(Value &lv, Value &rv) override;
  virtual llvm::Value *lt(Value &lv, Value &rv) override;
  virtual llvm::Value *le(Value &lv, Value &rv) override;
  virtual llvm::Value *gt(Value &lv, Value &rv) override;
  virtual llvm::Value *ge(Value &lv, Value &rv) override;
};

class BooleanTyTrait : virtual public Boolean {
  virtual std::string name() override { return "BooleanTyTrait"; }
  virtual llvm::Value *eq(Value &lv, Value &rv) override;
  virtual llvm::Value *ne(Value &lv, Value &rv) override;
};

} // namespace Compiler
