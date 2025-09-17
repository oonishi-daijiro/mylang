#pragma once

#include "debug.hpp"
#include "string_enum.hpp"
#include "text.hpp"

namespace Compiler {
#define OPERATOR                                                               \
  "lt", "gt", "le", "ge", "neq", "add", "sub", "div", "mul", "assign", "eq",   \
      "increment", "decrement"
//  lt: <
//  gt: >
//  le: <=
//  ge: >=

#define LITERAL                                                                \
  "double_literal", "string_literal", "boolean_literal", "integer_literal"

#define BUILTIN_TYPE "numeric", "char", "string", "boolean", "integer", "void"

#define DELIMITER                                                              \
  "left_paren", "right_paren", "semicolon", "begin_block", "end_block"

#define KEYWORD                                                                \
  "constdecl", "vardecl", "function", "return_stmt", "if_stmt",                \
      "else_if_stmt", "else_stmt", "for", "while", "break", "continue"

#define SENTINEL "bof", "eof"
#define SYMBOL "symbol"

using token_kind = string_enum<OPERATOR, LITERAL, BUILTIN_TYPE, DELIMITER,
                               KEYWORD, SENTINEL, SYMBOL>;

class Token {
public:
  token_kind kind;
  const DebugInfo info;
  const std::string value;

  Token() = delete;
  bool operator==(token_kind) const;
  bool operator!=(token_kind rt) const { return !(*this == rt); }
  Token(token_kind k, std::string &value, DebugInfo info);
};

class Tokennizer {
private:
  TextReader<char> reader;
  std::string_view source;

  template <size_t N> bool isIncluding(const auto (&p)[N], char v) {
    bool result = false;
    for (auto &&e : p) {
      result |= e == v;
    }
    return result;
  }

  bool isOperator(char o);
  bool isParentheses(char p);

public:
  Tokennizer() = delete;
  Tokennizer(std::string src);
  std::vector<Token> tokenize();
};
} // namespace Compiler
