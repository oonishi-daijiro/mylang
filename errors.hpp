#pragma once

#include "debug.hpp"
#include "token.hpp"

namespace Compiler {

class Error {
  std::string message;
  DebugInfo info;

public:
  Error(DebugInfo info, const std::string &message)
      : message{message}, info{info} {}
  const std::string &what() { return message; };
  const DebugInfo &getDebugInfo() { return info; }
};

class ParseError : public Error {
  using tokitr_t = std::vector<Token>::const_iterator;
  static inline tokitr_t *curtok;

public:
  ParseError(const std::string &message) : Error((*curtok)->info, message) {}
  ParseError(const DebugInfo &info, const std::string &message)
      : Error(info, message) {}
  static inline void init(tokitr_t &tokCursor) { curtok = &tokCursor; }
};

class SyntaxError : public ParseError {
public:
  using ParseError::ParseError;
  SyntaxError(const DebugInfo &info, const std::string &message)
      : ParseError{info, message} {}
};

class RangeError : public Error {
public:
  RangeError(const DebugInfo &info, const std::string &message)
      : Error{info, message} {}
};

class SymbolError : public ParseError {
public:
  using ParseError::ParseError;
};

class CodeGenError : public Error {
public:
  CodeGenError(const DebugInfo &info, const std::string message)
      : Error(info, message) {};
};

class CastError : public CodeGenError {
  using CodeGenError::CodeGenError;
};

class TypeError : public CodeGenError {
  using CodeGenError::CodeGenError;
};

} // namespace Compiler
