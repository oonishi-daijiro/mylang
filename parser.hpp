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

  // stmt     = (expr | if | ret | constdecl | vardecl | assign | funccall |
  // cmp_stmt);

  // constdecl = "const" symbol "=" expr
  // vardecl   = "let" symbol "=" expr
  // if       = "if" "(" expr ")" "{" cmp_stmt "}" (elif | else | ε)
  // assign   = symbol "=" expr
  // elif     = "else if" "(" expr ")" "{" cmp_stmt "}" (elif | else | ε)*
  // else     = "else" "{" cmp_stmt "}"
  // ret      = "return" epxr ";"

  // expr       = equality
  // equality   = relational ("==" relational | "!=" relational)*
  // relational = add ("<" add | ">" add | "<=" add | ">=" add)*
  // add        = mul ("+" mul | "-" mul)*
  // mul        = unary ("*" unary | "/" unary)*
  // unary      = ("+" | "-")? primary
  // primary    = (num | symbol | funccall) | "(" expr ")"

  Block *parseBlock(std::string name = "", llvm::Function *parent = nullptr);

  Statement *parseStatement();
  
  Statement *parseCompoundStatement();
  Statement *parseReturn();
  Statement *parseMutableVarDecl();
  Statement *parseConstantVarDecl();
  Statement *parseAssign();
  Statement *parseIfStmt();

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
