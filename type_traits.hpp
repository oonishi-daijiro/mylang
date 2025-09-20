#pragma once

#include "ast.hpp"
#include "type.hpp"
#include "utils.hpp"
#include "value.hpp"
#include <llvm/IR/Value.h>

namespace Compiler {

class AnyTrait : public TypeTrait {
public:
  virtual ~AnyTrait() = default;
  virtual std::string name() { return "any"; }
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

class Indexable : virtual public TypeTrait {
public:
  static inline std::string name{"Indexable"};
  virtual llvm::Value *at(Value &arraylike, Value &idx) = 0;
};

// implmentation of builtin types

class DoubleTyTrait : virtual public Ordered {
  Value *unitVal{nullptr};

public:
  virtual std::string name() override { return "DoubleTyTrait"; }
  virtual ~DoubleTyTrait();

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
  virtual ~IntegerTyTrait();

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
public:
  virtual std::string name() override { return "BooleanTyTrait"; }
  virtual llvm::Value *eq(Value &lv, Value &rv) override;
  virtual llvm::Value *ne(Value &lv, Value &rv) override;
};

class ArrayTyTrait : virtual public Indexable {
public:
  virtual std::string name() override { return "ArrayTyTrait"; }

  virtual llvm::Value *at(Value &arraylike, Value &idx) override;
};

} // namespace Compiler
