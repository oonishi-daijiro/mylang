#include <cctype>
#include <format>

#include "ast.hpp"
#include "debug.hpp"
#include "token.hpp"

namespace Compiler {

Token::Token(token_kind k, std::string &value, DebugInfo info)
    : kind{k}, value{value}, info{info} {};

bool Token::operator==(token_kind k) const { return this->kind == k; }

bool Tokennizer::isOperator(char o) {
  return isIncluding({'+', '-', '*', '/', '=', '>', '<', '!'}, o);
};

bool Tokennizer::isParentheses(char p) {
  return isIncluding({'(', ')', '{', '}'}, p);
};

Tokennizer::Tokennizer(std::string src) : reader{src}, source{src} {}

std::vector<Token> Tokennizer::tokenize() {
  std::vector<Token> tokens{};

  std::string ws{""};
  tokens.emplace_back(token_kind::of<"bof">, ws, 0);
  reader.skipws();

  while (!reader.isReachedEnd()) {
    std::string valueBuffer;
    decltype(reader)::character_t c = *reader;

    auto emplaceNewToken = [&](token_kind k, std::string value = "") {
      tokens.emplace_back(k, value, reader.getCursor());
    };

    if (std::isdigit(c)) {
      while (std::isalnum(*reader) || *reader == '.') {
        reader >>= c;
        ++reader;
        valueBuffer.push_back(c);
      }
      if (valueBuffer.contains('.')) {
        emplaceNewToken(token_kind::of<"double_literal">, valueBuffer);
      } else {
        emplaceNewToken(token_kind::of<"integer_literal">, valueBuffer);
      }
    } else if (c == '/' && reader + 1 == '/') {
      ++reader;
      while (*reader != '\n') {
        ++reader;
      }
    } else if (isOperator(c)) {
      token_kind k;
      if (c == '+') {
        k = token_kind::of<"add">;
      } else if (c == '-') {
        k = token_kind::of<"sub">;
      } else if (c == '*') {
        k = token_kind::of<"mul">;
      } else if (c == '/') {
        k = token_kind::of<"div">;
      } else if (c == '=' && (reader + 1) != '=') {
        k = token_kind::of<"assign">;
      } else if (c == '=' && (reader + 1) == '=') {
        k = token_kind::of<"eq">;
        ++reader;
      } else if (c == '<') {
        if (reader + 1 == '=') {
          // <=
          k = token_kind::of<"le">;
          ++reader;
        } else {
          // <
          k = token_kind::of<"lt">;
        }
      } else if (c == '>') {
        if (reader + 1 == '=') {
          // >=
          k = token_kind::of<"ge">;
          ++reader;
        } else {
          // >
          k = token_kind::of<"gt">;
        }
      } else if (c == '!') {
        //!=
        if (reader + 1 == '=') {
          k = token_kind::of<"neq">;
          ++reader;
        }
      }
      emplaceNewToken(k);
      ++reader;
    } else if (isParentheses(c)) {
      if (c == '(') {
        emplaceNewToken(token_kind::of<"left_paren">);
      } else if (c == ')') {
        emplaceNewToken(token_kind::of<"right_paren">);
      } else if (c == '{') {
        emplaceNewToken(token_kind::of<"begin_block">);
      } else if (c == '}') {
        emplaceNewToken(token_kind::of<"end_block">);
      }
      ++reader;
    } else if (std::isalpha(c)) {
      do {
        valueBuffer += c;
        ++reader;
        reader >>= c;
      } while (std::isalpha(c));

      if (valueBuffer == "const") {
        emplaceNewToken(token_kind::of<"constdecl">);
      } else if (valueBuffer == "function") {
        emplaceNewToken(token_kind::of<"function">);
      } else if (valueBuffer == "if") {
        emplaceNewToken(token_kind::of<"if_stmt">);
      } else if (valueBuffer == "else") {
        std::string next;
        reader >>= next;
        if (next == "if") {
          emplaceNewToken(token_kind::of<"else_if_stmt">);
          reader >> valueBuffer;
        } else {
          emplaceNewToken(token_kind::of<"else_stmt">);
        }
      } else if (valueBuffer == "return") {
        emplaceNewToken(token_kind::of<"return_stmt">);
      } else if (valueBuffer == "let") {
        emplaceNewToken(token_kind::of<"vardecl">);
      } else if (valueBuffer == "true") {
        emplaceNewToken(token_kind::of<"boolean_literal">, "true");
      } else if (valueBuffer == "false") {
        emplaceNewToken(token_kind::of<"boolean_literal">, "false");
      } else {
        emplaceNewToken(token_kind::of<"symbol">, valueBuffer);
      }
    } else if (c == ';') {
      emplaceNewToken(token_kind::of<"semicolon">);
      ++reader;
    } else if (c == '"') {
      ++reader;
      while (*reader != '"') {
        valueBuffer += *reader;
        ++reader;
      }
      ++reader;
      emplaceNewToken(token_kind::of<"string_literal">, valueBuffer);
    } else {
      ++reader;
    }
  }
  std::string emptyValue{};
  tokens.emplace_back(token_kind::of<"eof">, emptyValue, reader.getCursor());
  return tokens;
}
} // namespace Compiler
