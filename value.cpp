#include "value.hpp"

namespace Compiler {

ConstantValue::ConstantValue(llvm::Value *val, const std::string &tyName)
    : val{val} {
  type = tyName;
}
llvm::Value *ConstantValue::get() { return val; }

} // namespace Compiler
