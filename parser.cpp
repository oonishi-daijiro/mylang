#include <cstdlib>
#include <format>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Error.h>

#include "ast.hpp"
#include "control_statements.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "operators.hpp"
#include "parser.hpp"
#include "statements.hpp"
#include "token.hpp"

namespace Compiler {

Block *Parser::parseBlock(std::string name, llvm::Function *parent) {
  expect(token_kind::of<"begin_block">);
  auto blk = new Block(parseCompoundStatement(), name, parent);
  expect(token_kind::of<"end_block">);
  return blk;
}

Statement *Parser::parseStatement() {
  Statement *stmt = nullptr;

  if (consume(token_kind::of<"vardecl">)) {
    stmt = parseMutableVarDecl();
    expect(token_kind::of<"semicolon">);
  } else if (consume(token_kind::of<"constdecl">)) {
    stmt = parseConstantVarDecl();
    expect(token_kind::of<"semicolon">);
  } else if (consume(token_kind::of<"return_stmt">)) {
    stmt = parseReturn();
    expect(token_kind::of<"semicolon">);
  } else if (match(token_kind::of<"symbol">, token_kind::of<"assign">)) {
    stmt = parseAssign();
    expect(token_kind::of<"semicolon">);
  } else if (consume(token_kind::of<"if_stmt">)) {
    stmt = parseIfStmt();
  } else if (consume(token_kind::of<"begin_block">)) {
    stmt = parseCompoundStatement();
    expect(token_kind::of<"end_block">);
  } else if (consume(token_kind::of<"for">)) {
    stmt = parseForStmt();
  } else if (consume(token_kind::of<"continue">)) {
    stmt = new ContinueStatement();
    expect(token_kind::of<"semicolon">);
  } else if (consume(token_kind::of<"break">)) {
    stmt = new BreakStatement();
    expect(token_kind::of<"semicolon">);
  } else {
    stmt = parseExpression();
    expect(token_kind::of<"semicolon">);
  }

  return stmt;
}

Statement *Parser::parseCompoundStatement() {
  std::vector<Statement *> stmts{};
  while (*(tokitr + 1) != token_kind::of<"end_block">) {
    stmts.emplace_back(parseStatement());
  }
  auto cmstmt = new CompoundStatement(std::move(stmts));
  return cmstmt;
};

Statement *Parser::parseIfStmt() {
  expect(token_kind::of<"left_paren">);
  auto cond = parseExpression();
  expect(token_kind::of<"right_paren">);
  auto then = parseStatement();
  Statement *els = nullptr;
  if (consume(token_kind::of<"else_if_stmt">)) {
    els = parseIfStmt();
  } else if (consume(token_kind::of<"else_stmt">)) {
    els = parseStatement();
  }
  return new IfStatement(cond, then, els);
}

Statement *Parser::parseForStmt() {
  Statement *initial = nullptr;
  Expression *continueCond = nullptr;
  Expression *nextInit = nullptr;

  expect(token_kind::of<"left_paren">);
  if (consume(token_kind::of<"vardecl">)) {
    initial = parseMutableVarDecl();
  } else {
    initial = parseExpression();
  }
  expect(token_kind::of<"semicolon">);
  continueCond = parseExpression();
  expect(token_kind::of<"semicolon">);
  nextInit = parseExpression();
  expect(token_kind::of<"right_paren">);
  auto loopBody = parseStatement();
  return new ForStatement(initial, continueCond, nextInit, loopBody);
}

Statement *Parser::parseWhileStmt() {};

Statement *Parser::parseMutableVarDecl() {
  try {
    expect(token_kind::of<"symbol">);
    auto symbol = *tokitr;
    expect(token_kind::of<"assign">);
    auto expr = parseExpression();
    return new MutableVarDeclaration(symbol.value, *expr);
  } catch (std::string err) {
    throw SyntaxError(err);
  }
}

Statement *Parser::parseReturn() { return new Ret(parseExpression()); }

Statement *Parser::parseAssign() {
  try {
    auto symbol = tokitr - 2;
    auto lv = new Variable(symbol->value);
    auto rv = parseExpression();
    auto assign = new Assign(lv, rv);
    return assign;
  } catch (std::string err) {
    throw SyntaxError(err);
  }
}
Statement *Parser::parseConstantVarDecl() { return nullptr; }

Expression *Parser::parseExpression() { return parseEquality(); }

Expression *Parser::parseEquality() {
  Expression *equality = parseRelational();
  while (true) {
    if (consume(token_kind::of<"eq">)) {
      equality = new EqOperator(equality, parseRelational());
    } else if (consume(token_kind::of<"neq">)) {
      equality = new NeqOperator(equality, parseRelational());
    } else {
      return equality;
    }
  }
}

Expression *Parser::parseRelational() {
  Expression *relational = parseAdd();
  while (true) {
    if (consume(token_kind::of<"lt">)) { // <
      relational = new LtOperator(relational, parseAdd());
    } else if (consume(token_kind::of<"gt">)) { // >
      relational = new GtOperator(relational, parseAdd());
    } else if (consume(token_kind::of<"le">)) { // <=
      relational = new LtOperator(relational, parseAdd());
    } else if (consume(token_kind::of<"ge">)) { // >=
      relational = new GeOperator(relational, parseAdd());
    } else {
      return relational;
    }
  }
}

Expression *Parser::parseAdd() {
  Expression *mul = parseMul();
  while (true) {
    if (consume(token_kind::of<"add">)) {
      mul = new AddOperator(mul, parseMul());
    } else if (consume(token_kind::of<"sub">)) {
      mul = new SubOperator(mul, parseMul());
    } else {
      return mul;
    }
  }
}

Expression *Parser::parseMul() {
  Expression *unary = parseUnary();
  while (true) {
    if (consume(token_kind::of<"mul">)) {
      unary = new MulOperator(unary, parseUnary());
    } else if (consume(token_kind::of<"div">)) {
      unary = new DivOperator(unary, parseUnary());
    } else {
      return unary;
    }
  }
};

Expression *Parser::parseUnary() {
  Expression *node = nullptr;
  if (consume(token_kind::of<"add">)) {
    node = parsePrimary();
  } else if (consume(token_kind::of<"sub">)) {
    node = new MinusOperator(parsePrimary());
  } else {
    node = parsePrimary();
    if (consume(token_kind::of<"increment">)) {
      node = new IncrementOperator(node);
    } else if (consume(token_kind::of<"decrement">)) {
      node = new DecrementOperator(node);
    }
  }
  return node;
}

Expression *Parser::parsePrimary() {
  Expression *primary = nullptr;
  if (consume(token_kind::of<"left_paren">)) {
    primary = parseExpression();
    expect(token_kind::of<"right_paren">);
  } else if (consume(token_kind::of<"double_literal">)) {
    primary = new DoubleExpr(stodnoe(tokitr->value));
  } else if (consume(token_kind::of<"integer_literal">)) {
    primary = new IntegerExpr(std::atoi(tokitr->value.c_str()));
  } else if (consume(token_kind::of<"symbol">)) {
    primary = new Variable(tokitr->value);
  } else if (consume(token_kind::of<"boolean_literal">)) {
    primary = new BooleanExpr(tokitr->value == "true" ? true : false);
  } else {
    std::string err =
        std::format("[unexpected token] expected: numeric or symbol but {}",
                    tokitr->kind.to_string());
    throw SyntaxError(err);
  }
  return primary;
};

double Parser::stodnoe(auto &&str) noexcept {
  try {
    return std::stod(str);
  } catch (std::invalid_argument &e) {
    return 0;
  }
}

Parser::Parser(std::vector<Token> &&tokens)
    : tokens{std::forward<decltype(tokens)>(tokens)} {};

Root Parser::parse(llvm::Function *mainFunc) {
  Node::init(tokitr);
  ParseError::init(tokitr);
  Root root{parseBlock("", mainFunc)};
  return root;
};

bool Parser::consume(token_kind k) {
  if (*(tokitr + 1) == k) {
    ++tokitr;
    return true;
  } else {
    return false;
  }
}

bool Parser::expect(token_kind k) noexcept(false) {
  if (*(tokitr + 1) == k) {
    ++tokitr;
    return true;
  } else {
    throw SyntaxError{
        std::format(R""(unexpected token. expected "{}" but "{}")"",
                    k.to_string(), tokitr->kind.to_string())};
  }
}

} // namespace Compiler
