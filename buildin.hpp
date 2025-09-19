

#include <llvm-c/Types.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "kind.hpp"
#include "type.hpp"
#include "type_traits.hpp"

using namespace Compiler;

class BuiltinTypes : public Code {
public:
  static inline void define() {
    Type::DefineNewType<DoubleTyTrait>("double",
                                       llvm::Type::getDoubleTy(*context));
    Type::DefineNewType<IntegerTyTrait>("integer",
                                        llvm::Type::getInt32Ty(*context));
    Type::DefineNewType<BooleanTyTrait>("boolean",
                                        llvm::IntegerType::get(*context, 1));
    Type::DefineNewType<AnyTrait>("void", llvm::Type::getVoidTy(*context));
    Type::DefineNewType<AnyTrait>("char", llvm::IntegerType::get(*context, 8));

  }
  BuiltinTypes() = delete;
};
