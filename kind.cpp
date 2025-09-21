#include "kind.hpp"
#include "type.hpp"
#include "type_traits.hpp"

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

StringKind::StringKind(size_t s) : ArrayKind{Type::GetType("char"), s} {};
Type StringKind::Apply(size_t size) {
  auto arrayHeadPtrInst =
      llvm::PointerType::get(Type::GetType("char").getTypeInst(), 0);
  auto tyName = std::format("string");
  auto tr = TypeTrait::New<StringTyTrait>();
  auto k = Kind::New<StringKind>(size);
  return {tyName, arrayHeadPtrInst, tr, k};
};

}; // namespace Compiler
