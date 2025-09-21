#include "utils.hpp"
#include <fstream>

namespace util {
std::pair<std::unique_ptr<char[]>, size_t> readFile(std::string_view filepath) {
  std::ifstream file(filepath.data());
  if (file.fail()) {
    std::unique_ptr<char[]> fpNull(new char[0]);
    return {std::move(fpNull), 0};
  }

  file.seekg(0, std::ios::end);
  size_t fileLength = file.tellg();

  auto fp = std::make_unique<char[]>(fileLength + 1);
  file.seekg(0, std::ios::beg);
  file.read(fp.get(), fileLength);
  fp[fileLength] = '\0';
  return {std::move(fp), fileLength};
}

void writeToFile(std::string_view filename, llvm::Module &mainModule) {
  std::error_code EC;
  llvm::raw_fd_ostream outfile(filename, EC, llvm::sys::fs::OF_Text);
  mainModule.print(outfile, nullptr);
}

void printErrorSourceLine(std::string &source, Compiler::Error &error) {
  int newLineIndex = 0;
  size_t indentLen = 0;
  int linePos = 0;

  for (size_t i = 0; i < source.size() && i < error.getDebugInfo().tokenIndex;
       i++) {
    indentLen++;
    if (source[i] == '\n') {
      linePos++;
      newLineIndex = i;
      indentLen = 0;
    }
  }
  std::cout << std::endl;
  std::cout << error.what() << '\n';
  std::cout << "=============== COMPILE ERROR AT ============" << std::endl;
  std::cout << "  |" << std::endl;

  std::cout << linePos + 1 << " | ";

  for (size_t i = newLineIndex + 1; i <= source.size() && source[i] != '\n';
       i++) {
    std::cout << source[i];
  }

  std::cout << '\n';
  for (size_t i = 0; i < indentLen; i++) {
    if (i == std::to_string(linePos + 1).size() + 1) {
      std::cout << '|';
    } else {
      std::cout << ' ';
    }
  }
  std::cout << "^ here" << std::endl;
}

llvm::orc::ThreadSafeModule
createThreadSafeModule(std::unique_ptr<llvm::LLVMContext> &&context,
                       std::string moduleName) {
  auto mainModule = std::make_unique<llvm::Module>(moduleName, *context);
  llvm::orc::ThreadSafeModule tsm{std::move(mainModule), std::move(context)};
  return tsm;
}

std::unique_ptr<llvm::orc::LLJIT>
createCompiler(llvm::orc::ThreadSafeModule &&module) noexcept(false) {
  auto JITcompiler = llvm::orc::LLJITBuilder().create();
  if (JITcompiler.takeError()) {
    throw std::runtime_error(
        std::format("failed to create JIT compiler{}",
                    llvm::toString(JITcompiler.takeError())));
  }

  auto res =
      JITcompiler.get()->addIRModule(std::forward<decltype(module)>(module));

  if (res) {
    throw std::runtime_error("failed to add IR module to JIT");
  }
  return std::move(JITcompiler.get());
}

} // namespace util
