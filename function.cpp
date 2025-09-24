#include "function.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "kind.hpp"
#include "scope.hpp"
#include "statement.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "value.hpp"
#include <format>
#include <functional>
#include <llvm/IR/Value.h>

namespace Compiler {

// FunctionSignature
FunctionSignature::FunctionSignature(const std::vector<ArgumentInfo> &&argument,
                                     const std::optional<Type> &ret)
    : ret{ret}, args{argument} {
  for (auto &&arg : args) {
    argTy.emplace_back(&arg.type());
  }
}

bool FunctionSignature::operator==(const FunctionSignature &signature) const {
  bool isSameSignature = ret == signature.ret;
  isSameSignature &= args.size() == signature.args.size();
  if (!isSameSignature) {
    return false;
  }

  for (size_t i = 0; i < args.size(); i++) {
    isSameSignature &= args[i].type() == signature.args[i].type();
    if (!isSameSignature) {
      break;
    }
  }
  return isSameSignature;
}

const std::optional<Type> FunctionSignature::returnType() const { return ret; }
void FunctionSignature::setInferedReturnType(const Type &t) { ret = t; }
const decltype(FunctionSignature::args) &FunctionSignature::arguments() const {
  return args;
}
const std::vector<const Type *> &FunctionSignature::argType() const {
  return argTy;
}

const std::string FunctionSignature::to_string() const {
  std::stringstream ss;

  ss << '(';
  for (int i = 0; i < args.size(); i++) {
    auto &&name = args[i].name();
    auto &&type = args[i].type();
    ss << std::format("{}:{}", name, type.name())
       << ((i != args.size() - 1) ? "," : "");
  }
  ss << std::format(") -> {}", ret ? ret->name() : "void");
  return ss.str();
}

// FunctionArgument

FunctionArgument::FunctionArgument(const std::string &name, const Type &type)
    : name{name}, initTy{type}, Variable(name) {};

void FunctionArgument::resolveSymbol() {
  if (currentScope().existsOnSameScope(name)) {
    throw SymbolError(
        info, std::format("argument name \"{}\" is already used.", name));
  } else {
    registerToCurrentScope();
  }
}

void FunctionArgument::resolveType() { type = initTy; }

llvm::Value *FunctionArgument::get() {
  return builder->CreateLoad(type, argCopy);
};

void FunctionArgument::set(Value &val) {
  builder->CreateStore(val.get(), argCopy);
}

llvm::Value *FunctionArgument::ptr() { return argCopy; }

const std::string FunctionArgument::kind() const {
  return std::format("function argment {}", name);
}

void FunctionArgument::setArgSubstance(llvm::Value *sub) {
  argSubstance = sub;
};

// Function

Function::Function(const std::string &name, const FunctionSignature &sig,
                   Block *body)
    : body{*body}, sig{sig}, name{name}, Symbol{name} {
  for (auto &&info : sig.arguments()) {
    auto arg = new FunctionArgument(info.name(), info.type());
    argments.emplace_back(arg);
    appendChild(arg);
  }
  appendChild(body);
}

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
  ty = FunctionKind::Apply(sig);
}

void Function::gen() {
  auto bb = llvm::BasicBlock::Create(*context, name, func);
  builder->SetInsertPoint(bb);
  for (auto &&arg : argments) {
    arg->arg2local();
  }
  body.setParentFunc(func);
  body.gen();
}

void Function::resolveScope() { walkAllChildlenBF(defaultScopeInitalizer); }

void Function::resolveSymbol() {
  if (currentScope().existsOnSameScope(name)) {
    throw SymbolError(info,
                      std::format("symbol \"{}\" is already defined as", name,
                                  currentScope().find(name)->kind()));
  } else {
    registerToCurrentScope();
  }
}

void Function::init() {
  std::vector<llvm::Type *> argtypes{};
  for (auto &&t : sig.argType()) {
    argtypes.emplace_back(t->getTypeInst());
  }
  auto funcType =
      llvm::FunctionType::get(sig.returnType()->getTypeInst(), argtypes, false);
  func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name,
                                *llvmModule);
  auto args = func->args();
  int i = 0;
  for (auto &&llArg : args) {
    llArg.setName(sig.arguments()[i].name());
    i++;
  }

  for (int i = 0; i < sig.arguments().size(); i++) {
    argments[i]->setArgSubstance(func->getArg(i));
  }
}

llvm::Value *Function::funcPtr() { return func; }
const Type &Function::type() { return ty; }

}; // namespace Compiler
