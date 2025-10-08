

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
    Type::DefineNewPrimitiveType("double", llvm::Type::getDoubleTy(*context),
                                 TypeTrait::New<DoubleTyTrait>());
    Type::DefineNewPrimitiveType("integer", llvm::Type::getInt32Ty(*context),
                                 TypeTrait::New<IntegerTyTrait>());
    Type::DefineNewPrimitiveType("boolean", llvm::IntegerType::get(*context, 1),
                                 TypeTrait::New<BooleanTyTrait>());

    auto any = TypeTrait::New<AnyTrait>();
    Type::DefineNewPrimitiveType("void", llvm::Type::getVoidTy(*context), any);
    Type::DefineNewPrimitiveType("char", llvm::IntegerType::get(*context, 8),
                                 any);
    Type::DefineNewPrimitiveType(
        "string",
        llvm::PointerType::get(Type::GetType("char").getTypeInst(), 0),
        TypeTrait::New<StringTyTrait>());

    // Kind::DefineNewKind<ArrayKind>("array");
    // Kind::DefineNewKind<PointerKind>("pointer");
    // Kind::DefineNewKind<PrimitiveKind>("primitive");
  }
  BuiltinTypes() = delete;
};
