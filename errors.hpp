#pragma once
#include <stdexcept>

namespace Compiler {

class Error : public std::runtime_error {
public:
  Error(const std::string &message) : std::runtime_error(message) {}
};

struct CompileError : public std::runtime_error {
  CompileError(size_t tokenIndex, Error &err)
      : errorIndex{tokenIndex}, std::runtime_error{err.what()} {};
  size_t errorIndex;
};

class TypeError : public Error {
public:
  TypeError(const std::string &message) : Error("TypeError: " + message) {}
};

class GenerationRuntimeError : public Error {
public:
  GenerationRuntimeError(const std::string &message)
      : Error("GenerationRuntimeError: " + message) {}
};

class SyntaxError : public Error {
public:
  SyntaxError(const std::string &message) : Error("SyntaxError: " + message) {}
};

class ParseError : public Error {
public:
  ParseError(const std::string &message) : Error("ParseError:" + message) {}
};

class CastError : public Error {
public:
  CastError(const std::string &message) : Error("CastError: " + message) {}
};

} // namespace Compiler
