#pragma once

#include "ast.hpp"
#include "expressions.hpp"
#include "token.hpp"

namespace Compiler {
class Parser {

  const std::vector<Token> tokens;
  using token_iterator =
      std::remove_reference_t<decltype(tokens)>::const_iterator;

  token_iterator tokitr{tokens.begin()};

  bool consume(token_kind k);
  bool expect(token_kind k) noexcept(false);

  template <typename... T> bool match(T... k) {
    bool matched = false;
    token_kind toks[sizeof...(k)] = {k...};
    for (size_t i = 1; i <= sizeof...(k); i++) {
      matched &= (*(tokitr + i)).kind == toks[i];
    }
    if (matched) {
      tokitr += sizeof...(k);
    }
    return matched;
  };

  double stodnoe(auto &&) noexcept;

  // program  = function*

  // symbol   = @regexp: ^[a-zA-Z_][a-zA-Z0-9_]*$

  // function = symbol "(" argdef ")" block
  // argdef   = (symbol adtail* | ε)
  // adtail   = "," symbol

  // funcarg  = (expr argtail*| ε)
  // argtail  = "," expr

  // funccall = symbol "(" funcarg ")"

  // cmp_stmt = ("{" stms "}")*

  // stmt     = (expr | if | ret | constdecl | vardecl | assign | funccall ) ";"
  //            | (cmp_stmt | for | while)

  // constdecl = "const" symbol "=" expr
  // vardecl   = "let" symbol "=" expr
  // if       = "if" "(" expr ")" "{" cmp_stmt "}" (elif | else | ε)
  // assign   = symbol "=" expr
  // elif     = "else if" "(" expr ")" "{" cmp_stmt "}" (elif | else | ε)*
  // else     = "else" "{" cmp_stmt "}"
  // ret      = "return" epxr ";"
  // for      = "for" "(" (vardecl|expr) ";" expr ";" expr ")" stmt
  // while    = "while" "(" expr ")" stmt

  // expr       = equality
  // equality   = relational ("==" relational | "!=" relational)*
  // relational = add ("<" add | ">" add | "<=" add | ">=" add)*
  // add        = mul ("+" mul | "-" mul)*
  // mul        = unary ("*" unary | "/" unary)*
  // unary      = (("+" | "-")? primary)| primary++ | primary--
  // primary    = (num | symbol | funccall) | "(" expr ")"

  Block *parseBlock(std::string name = "", llvm::Function *parent = nullptr);

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

public:
  Parser() = delete;
  Parser(std::vector<Token> &&tokens);

  Root parse(llvm::Function *mainfunc);
};
} // namespace Compiler
