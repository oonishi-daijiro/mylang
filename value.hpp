#pragma once

#include <llvm/IR/Value.h>

#include "type.hpp"

namespace Compiler {

class Value {
public:
  virtual ~Value() = default;
  virtual llvm::Value *get() = 0;

  Value() = default;
  Type type{};
};

class ConstantValue final : public Value {
  llvm::Value *val{nullptr};

public:
  ConstantValue(llvm::Value *val, const std::string &tyName);
  virtual ~ConstantValue() = default;
  virtual llvm::Value *get() final override;
};

} // namespace Compiler
