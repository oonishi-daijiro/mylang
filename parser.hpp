#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "ast.hpp"
#include "block.hpp"
#include "expressions.hpp"
#include "function.hpp"
#include "program.hpp"
#include "token.hpp"

namespace Compiler {
class Parser {

  const std::vector<Token> tokens;
  using token_iterator =
      std::remove_reference_t<decltype(tokens)>::const_iterator;

  token_iterator tokitr{tokens.begin()};

  bool consume(token_kind k);
  bool expect(token_kind k) noexcept(false);

  template <typename S, typename... T> struct match_impl {
    static inline bool match(token_iterator &itr, T &...) {
      throw std::runtime_error("this match function cannot be call");
    }
  };

  template <int... N, typename... T>
  struct match_impl<std::index_sequence<N...>, T...> {
    static inline bool match(token_iterator &itr, T &...k) {
      return (((itr + N + 1)->kind == k) && ...);
    }
  };

  template <typename... T> bool match(T &...k) {
    auto val = match_impl<std::make_index_sequence<sizeof...(T)>, T...>::match(
        tokitr, k...);
    if (val) {
      tokitr += sizeof...(T);
    }
    return val;
  };

  double stodnoe(auto &&) noexcept;

  // program  = function*

  // symbol   = @regexp: ^[a-zA-Z_][a-zA-Z0-9_]*$

  // function = "function" symbol "(" (arg | ε) ")" ((":" type_specifier) | ε)
  //             block

  // arg      = (symbol ":" type_specifier arg_tail)
  // arg_tail = ("," arg) | ε

  // funcarg  = (expr argtail*| ε)
  // argtail  = "," expr

  // funccall = symbol "(" funcarg ")"

  // cmp_stmt = ("{" stms "}")*

  // stmt     = (expr | if | ret | constdecl | vardecl | assign | funccall ) ";"
  //            | (cmp_stmt | for | while)

  // constdecl  = "const" symbol "=" expr
  // vardecl    = "let" symbol "=" expr

  // if         = "if" "(" expr ")" "{" cmp_stmt "}" (elif | else | ε)
  // assign     = symbol "=" expr
  // elif       = "else if" "(" expr ")" "{" cmp_stmt "}" (elif | else | ε)*
  // else       = "else" "{" cmp_stmt "}"
  // ret        = "return" epxr ";"
  // for        = "for" "(" (vardecl|expr) ";" expr ";" expr ")" stmt
  // while      = "while" "(" expr ")" stmt

  // array_literal  = "[" expr ("," expr | ε)* "]"
  // array_indexing = primary "[" expr "]"

  // expr       = equality
  // equality   = relational ("==" relational | "!=" relational)*
  // relational = add ("<" add | ">" add | "<=" add | ">=" add)*
  // add        = mul ("+" mul | "-" mul)*
  // mul        = unary ("*" unary | "/" unary)*
  // unary      = (("+" | "-")? primary)| primary++ | primary--
  // primary    = (literal | symbol | funccall | array_indexing | array_expr) |
  // "(" expr ")"

  // literal    = #doule_literal | #integer_literal | #string_literal |
  // array_literal
  // # above means token kind
  Program *parseProgram();

  Function *parseFunction();
  Block *parseBlock();

  Statement *parseStatement();

  Statement *parseCompoundStatement();
  Statement *parseReturn();
  Statement *parseMutableVarDecl();
  Statement *parseConstantVarDecl();
  Statement *parseAssign();
  Statement *parseIfStmt();
  Statement *parseForStmt();
  Statement *parseWhileStmt();

  Expression *parseExpression();

  Expression *parseEquality();
  Expression *parseRelational();
  Expression *parseAdd();
  Expression *parseMul();
  Expression *parseUnary();
  Expression *parsePrimary();
  Expression *parseArrayLiteral();

  Expression *parseIndexing(Expression *);
  Expression *parseCall(Expression *);
  std::vector<Expression *> parseCommaList(token_kind);

public:
  Parser() = delete;
  Parser(std::vector<Token> &&tokens);

  Root parse();
};
} // namespace Compiler
