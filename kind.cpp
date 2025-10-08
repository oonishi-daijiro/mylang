#include "kind.hpp"
#include "function.hpp"
#include "type.hpp"
#include "type_traits.hpp"
#include <algorithm>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <string>

namespace Compiler {
// PrimitiveKind
std::string PrimitiveKind::name() { return "primitive"; }
Type PrimitiveKind::Apply(const Type &t) {
  auto k = Kind::New<PrimitiveKind>();
  return {t.name(), t.getTypeInst(), t.trait(), k};
}

// ArrayKind

std::string ArrayKind::name() { return "array"; }

ArrayKind::ArrayKind(const Type &elmTy, size_t s)
    : elementTy{elmTy}, arraySize{s} {}

const Type &ArrayKind::element() { return elementTy; }
const size_t ArrayKind::size() { return arraySize; }

Type ArrayKind::Apply(const Type &t, size_t size) {
  auto arrayHeadPtrInst = llvm::PointerType::get(t.getTypeInst(), 0);
  auto tyName = std::format("array[{}]", size);
  auto tr = TypeTrait::New<ArrayTyTrait>();
  auto k = Kind::New<ArrayKind>(t, size);
  return {tyName, arrayHeadPtrInst, tr, k};
}

// StringKind

// StringKind::StringKind(size_t s) : ArrayKind{Type::GetType("char"), s} {};
// Type StringKind::Apply(size_t size) {
//   auto arrayHeadPtrInst =
//       llvm::PointerType::get(Type::GetType("char").getTypeInst(), 0);
//   auto tyName = std::format("string");
//   auto tr = TypeTrait::New<StringTyTrait>();
//   auto k = Kind::New<StringKind>(size);
//   return {tyName, arrayHeadPtrInst, tr, k};
// };

// FunctionKind
FunctionKind::FunctionKind(const FunctionSignature &sig, llvm::FunctionType *ty)
    : sig{sig}, funcTy{ty} {}

std::string FunctionKind::name() { return "function"; }

Type FunctionKind::Apply(const FunctionSignature &sig) {
  std::vector<llvm::Type *> argtype{};
  llvm::Type *retType = sig.returnType()->getTypeInst();
  std::string argStr;

  for (auto &&t : sig.arguments()) {
    argtype.emplace_back(t.type().getTypeInst());
  }

  auto funcType = llvm::FunctionType::get(retType, argtype, false);
  auto typeName = sig.to_string();
  auto tr = TypeTrait::New<FunctionTyTrait>();
  auto kind = Kind::New<FunctionKind>(sig, funcType);
  return {typeName, funcType->getPointerTo(), tr, kind};
}

llvm::FunctionType *FunctionKind::funcType() { return funcTy; }
const FunctionSignature &FunctionKind::signature() const { return sig; }

}; // namespace Compiler
