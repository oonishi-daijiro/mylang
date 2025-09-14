
#include "ast.hpp"
#include "traits.hpp"
#include <llvm-c/Types.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

using namespace Compiler;
class BuiltinTypes : public Code {
public:
  static inline void define() {
    Type::DefineNewType("double", llvm::Type::getDoubleTy(*context),
                        new DoubleTyTrait());
    Type::DefineNewType("integer", llvm::Type::getInt32Ty(*context),
                        new IntegerTyTrait());
    Type::DefineNewType("boolean", llvm::IntegerType::get(*context, 1),
                        new BooleanTyTrait());
    Type::DefineNewType("void", llvm::Type::getVoidTy(*context));

    Type::DefineNewType("char", llvm::IntegerType::get(*context, 8));
  }
  BuiltinTypes() = delete;
};
