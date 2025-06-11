#include <cstddef>
#include <cstdlib>
#include <exception>
#include <format>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>

#include "ast.hpp"
#include "errors.hpp"
#include "expressions.hpp"
#include "operators.hpp"
#include "parser.hpp"
#include "token.hpp"
#include "traits.hpp"

namespace Compiler {

Block *Parser::parseBlock(std::string name, llvm::Function *parent) {
  expect(token_kind::of<"begin_block">);
  std::vector<Statement *> stmts{};
  while (*(tokitr + 1) != token_kind::of<"end_block">) {
    stmts.emplace_back(parseStatement());
  }
  expect(token_kind::of<"end_block">);
  Block *block = new Block(std::move(stmts), name, parent);
  return block;
};

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
  } else if (consume(token_kind::of<"symbol">)) {
    if (consume(token_kind::of<"assign">)) {
      stmt = parseAssign();
    }
    expect(token_kind::of<"semicolon">);
  } else if (consume(token_kind::of<"if_stmt">)) {
    stmt = parseIfStmt();
  } else {
    stmt = parseExpression();
    expect(token_kind::of<"semicolon">);
  }

  return stmt;
}

Statement *Parser::parseIfStmt() {
  // expect(token_kind::of<"left_paren">);
  // auto cond = new ImplicitBooleanCast(parseExpression());
  // expect(token_kind::of<"right_paren">);
  // auto then = parseBlock("then");
  // IfStatement *el = nullptr;
  // if (consume(token_kind::of<"else_if_stmt">)) {
  //   el = parseIfStmt();
  // } else if (consume(token_kind::of<"else_stmt">)) {
  //   el = new ElseStatement(parseBlock("then"));
  // }
  // return new IfStatement(cond, then, el);
}

Statement *Parser::parseMutableVarDecl() {
  // try {
  //   expect(token_kind::of<"symbol">);
  //   auto symbol = *tokitr;
  //   expect(token_kind::of<"assign">);
  //   auto expr = parseExpression();
  //   return new MutableVarDecl(symbol.value, expr);
  // } catch (std::string err) {
  //   throw SyntaxError(err);
  // }
}

Statement *Parser::parseConstantVarDecl() {
  // try {
  //   expect(token_kind::of<"symbol">);
  //   auto symbol = *tokitr;
  //   expect(token_kind::of<"assign">);
  //   auto expr = parseExpression();
  //   return new ConstantVarDecl(symbol.value, expr);
  // } catch (std::string err) {
  //   throw SyntaxError(err);
  // }
}

Statement *Parser::parseReturn() {
  std::cout << "parse return" << std::endl;
  return new Ret(parseExpression());
}

Statement *Parser::parseAssign() {
  // try {
  //   auto symbol = tokitr - 1;
  //   auto lv = Variable::Get(symbol->value);
  //   auto rv = parseExpression();
  //   auto assign = new AssignOperator(lv, rv);
  //   return assign;
  // } catch (std::string err) {
  //   throw SyntaxError(err);
  // }
}

Expression *Parser::parseExpression() { return parseEquality(); }

Expression *Parser::parseEquality() {
  Expression *equality = parseRelational();
  while (true) {
    if (consume(token_kind::of<"eq">)) {
      equality = new EqOperator(equality->cast<Numeric>(), parseRelational());
    } else if (consume(token_kind::of<"neq">)) {
      equality = new NeOperator(equality->cast<Numeric>(), parseRelational());
    } else {
      return equality;
    }
  }
}

Expression *Parser::parseRelational() {
  Expression *relational = parseAdd();
  while (true) {
    if (consume(token_kind::of<"lt">)) { // <
      relational = new LtOperator(relational->cast<Numeric>(), parseAdd());
    } else if (consume(token_kind::of<"gt">)) { // >
      relational = new GtOperator(relational->cast<Numeric>(), parseAdd());
    } else if (consume(token_kind::of<"le">)) { // <=
      relational = new LtOperator(relational->cast<Numeric>(), parseAdd());
    } else if (consume(token_kind::of<"ge">)) { // >=
      relational = new GeOperator(relational->cast<Numeric>(), parseAdd());
    } else {
      return relational;
    }
  }
}

Expression *Parser::parseAdd() {
  Expression *mul = parseMul();
  while (true) {
    if (consume(token_kind::of<"add">)) {
      mul = new AddOperator(mul->cast<Numeric>(), parseMul());
    } else if (consume(token_kind::of<"sub">)) {
      mul = new Suboperator(mul->cast<Numeric>(), parseMul());
    } else {
      return mul;
    }
  }
}

Expression *Parser::parseMul() {
  Expression *unary = parseUnary();
  while (true) {
    if (consume(token_kind::of<"mul">)) {
      unary = new MulOperator(unary->cast<Numeric>(), parseUnary());
    } else if (consume(token_kind::of<"div">)) {
      unary = new DivOperator(unary->cast<Numeric>(), parseUnary());
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
    node = new MinusOperator(parsePrimary()->cast<Numeric>());
  } else {
    node = parsePrimary();
  }
  return node;
}

Expression *Parser::parsePrimary() {

  Numeric *node = nullptr;
  if (consume(token_kind::of<"left_paren">)) {
    node = parseExpression()->cast<Numeric>();
    expect(token_kind::of<"right_paren">);
  } else if (consume(token_kind::of<"double_literal">)) {
    node = new Double(stodnoe(tokitr->value));
  } else if (consume(token_kind::of<"integer_literal">)) {
    node = new Integer(std::atoi(tokitr->value.c_str()));
  } else if (consume(token_kind::of<"symbol">)) {
    // try {
    //   node = Variable::Get(tokitr->value);
    // } catch (std::string err) {
    //   throw SyntaxError(err);
    // }
  } else {
    std::string err =
        std::format("[unexpected token] expected: numeric or symbol but {}",
                    tokitr->kind.to_string());
    throw SyntaxError(err);
  }
  return node;
};

double Parser::stodnoe(auto &&str) noexcept {
  try {
    return std::stod(str);
  } catch (std::invalid_argument e) {
    return 0;
  } catch (std::exception e) {
    return 0;
  }
}

Parser::Parser(std::vector<Token> &&tokens)
    : tokens{std::forward<decltype(tokens)>(tokens)} {};

Root Parser::parse(llvm::Function *mainFunc) {
  try {
    Node::init(tokitr);
    Root root{parseBlock("", mainFunc)};
    return root;
  } catch (Error &err) {
    throw CompileError{tokitr->info.tokenIndex, err};
  }
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
                    k.to_string(), (tokitr + 1)->kind.to_string())};
  }
}

} // namespace Compiler
