#pragma once

#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/FileSystem.h>

#include <memory>
#include <string>

#include "errors.hpp"

namespace util {
std::pair<std::unique_ptr<char[]>, size_t> readFile(std::string_view filepath);

void writeToFile(std::string_view filename, llvm::Module &mainModule);
void printErrorSourceLine(std::string &source, Compiler::Error &error);

llvm::orc::ThreadSafeModule
createThreadSafeModule(std::unique_ptr<llvm::LLVMContext> &&context,
                       std::string moduleName);

template <typename T> auto safeCast(::llvm::Value *value) {
  if (::llvm::isa<T>(value)) {
    return ::llvm::cast<T>(value);
  } else {
    return nullptr;
  }
}

inline std::string lltos(auto *inst) {
  if (inst != nullptr) {
    std::string s;
    llvm::raw_string_ostream rso(s);
    inst->print(rso);
    rso.flush();
    return s;
  } else {
    return "[[[nullptr]]]";
  }
}

std::unique_ptr<llvm::orc::LLJIT>
createCompiler(llvm::orc::ThreadSafeModule &&module) noexcept(false);

template <typename T>
concept NOPTR_NOREF = requires() {
  !std::is_pointer_v<T>;
  !std::is_reference_v<T>;
};

} // namespace util
