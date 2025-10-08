
#include <cctype>
#include <cstring>

#include <ios>
#include <iostream>
#include <llvm/IR/Verifier.h>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "ast.hpp"
#include "buildin.hpp"
#include "errors.hpp"
#include "parser.hpp"
#include "token.hpp"
#include "utils.hpp"

#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetSelect.h>

int main(int argc, char **argv) {
  std::string source;
  llvm::InitLLVM init{argc, argv};

  if (argc > 1) {
    auto [f, l] = util::readFile(argv[1]);
    source.resize(l);
    std::memcpy(source.data(), f.get(), l);
  } else {
    std::cout << "input source as command arg pls." << std::endl;
    return 1;
  }

  try {
    auto context = std::make_unique<llvm::LLVMContext>();
    auto mainModule = std::make_unique<llvm::Module>("main", *context);

    llvm::IRBuilder<> builder{*context};
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    Compiler::LLVMBuilder::init(context.get(), mainModule.get());

    BuiltinTypes::define();

    std::string soucePath{(argc > 1 ? argv[1] : "../main.hoge")};
    Compiler::Tokennizer tokennizer{source};
    auto tokens = tokennizer.tokenize();
    std::cout << source << std::endl;

    // for (auto &&token : tokens) {
    //   std::cout << token.kind.to_string() << " : \"" << token.value << '"'
    //             << std::endl;
    // }

    Compiler::Parser parser{std::move(tokens)};

    auto root = parser.parse();

    root.gen();

    llvm::verifyModule(*mainModule);

    std::cout << "===============  LLVM IR  ================" << std::endl;
    mainModule->print(llvm::outs(), nullptr);
    std::cout << "==========================================" << std::endl;

    if (argc > 2) {
      util::writeToFile(argv[2], *mainModule);
    }

    llvm::orc::ThreadSafeModule tsm{std::move(mainModule), std::move(context)};
    auto compiler = util::createCompiler(std::move(tsm));
    std::cout << "create compiler" << std::endl;

    auto f = compiler->lookup("entry")->toPtr<int()>();
    std::cout << std::boolalpha;
    std::cout << "return:" << f() << std::endl;
  } catch (Compiler::Error &err) {
    util::printErrorSourceLine(source, err);
  } catch (std::runtime_error &e) {
    std::cout << "[RUNTIME ERROR]" << e.what() << std::endl;
  } catch (std::string &e) {
    std::cout << "std string error:" << e << std::endl;
  }
  return 0;
}
