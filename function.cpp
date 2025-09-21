#include "function.hpp"
#include "errors.hpp"
#include "scope.hpp"
#include "statement.hpp"
#include "symbol.hpp"
#include <format>

namespace Compiler {

// FunctionSignature
FunctionSignature::FunctionSignature(
    const std::vector<std::pair<std::string, Type>> &&argument,
    const std::optional<Type> &ret)
    : ret{ret}, argument{argument} {
  for (auto &&[_, t] : argument) {
    argTy.emplace_back(t);
  }
}

bool FunctionSignature::operator==(const FunctionSignature &signature) const {
  bool isSameSignature = ret == signature.ret;
  isSameSignature &= argument.size() == signature.argument.size();
  if (!isSameSignature) {
    return false;
  }

  for (size_t i = 0; i < argument.size(); i++) {
    isSameSignature &= argument[i].second == signature.argument[i].second;
    if (!isSameSignature) {
      break;
    }
  }
  return isSameSignature;
}

const std::optional<Type> FunctionSignature::returnType() const { return ret; }
void FunctionSignature::setInferedReturnType(const Type &t) { ret = t; }
const std::vector<Type> &FunctionSignature::argType() const { return argTy; }

std::string FunctionSignature::to_string() {
  std::stringstream ss;
  ss << '(';
  for (int i = 0; i < argument.size(); i++) {
    auto &&[name, type] = argument[i];
    ss << std::format("{}:{}", name, type.name())
       << ((i != argument.size() - 1) ? "," : "");
  }
  ss << std::format(") -> {}", ret->name());
  return ss.str();
}

// Function

Type Function::inferReturnType() {
  const Type *retType = nullptr;

  body.walkAllChildlenDFPO([&](Node *n) {
    if (n->isa<Ret>()) {
      auto ret = n->cast<Ret>();
      if (retType != nullptr && ret->returnType() != *retType) {
        throw TypeError(ret->info,
                        std::format("return type missmatching {} vs {}",
                                    retType->name(), ret->returnType().name()));
      } else {
        retType = &ret->returnType();
      }
    }
  });

  if (retType == nullptr) {
    return Type::GetType("void");
  } else {
    return *retType;
  }
}

void Function::resolveType() {
  auto infered = inferReturnType();
  if (sig.returnType() && (infered != sig.returnType().value())) {
    throw TypeError(info,
                    std::format("return type misssmatching {} vs {}",
                                infered.name(), sig.returnType()->name()));
  } else if (!sig.returnType()) {
    sig.setInferedReturnType(infered);
  }

  body.setReturnType(sig.returnType().value());
}

Function::Function(const std::string &name, const FunctionSignature &sig,
                   Block *body)
    : Node{body}, body{*body}, sig{sig}, name{name}, Symbol{name} {}

void Function::gen() {
  std::vector<llvm::Type *> argtypes{};
  for (auto &&t : sig.argType()) {
    argtypes.emplace_back(t.getTypeInst());
  }
  auto funcType =
      llvm::FunctionType::get(sig.returnType()->getTypeInst(), argtypes, false);
  auto func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                     name, *llvmModule);

  body.setParentFunc(func);
  body.gen();
}

void Function::resolveScope() {
  walkAllChildlenBF([&](Node *n) {
    if (n->isa<CompoundStatement>()) {
      auto cmpstmt = n->cast<CompoundStatement>();
      cmpstmt->scope().setParent(scp);
    } else if (n->isa<Symbol>()) {
      auto symbol = n->cast<Symbol>();
      symbol->setScope(scp);
    }
  });
}

void Function::resolveSymbol() {
  if (scope().exists(name)) {
    throw SymbolError(info,
                      std::format("symbol \"{}\" is already defined", name));
  }
}
}; // namespace Compiler
