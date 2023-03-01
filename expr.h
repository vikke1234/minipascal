/**
 * This file contains definitions for all symbols that the parser needs.
 */
#ifndef EXPR_H
#define EXPR_H
#include <memory>
#include <string>
#include <variant>

#include "token.h"
#include "lexer.h"


class Expr {
protected:
    std::unique_ptr<Token> token;
public:
    Expr(std::unique_ptr<Token> t) : token{std::move(t)} {}
    Expr() = default;
    virtual ~Expr() = default;

    virtual void interpet(void) = 0;
    virtual void visit(void) = 0;
};

class Statement : public Expr {
    token_type type;
    Statement * next;

public:
    Statement(std::unique_ptr<Token> t) : Expr(std::move(t)) { }
    virtual void interpet(void) override {}
    virtual void visit(void) override  {}
};

class StatementList : public Expr {
    std::unique_ptr<Expr> statement;
    std::unique_ptr<StatementList> next;

public:
    StatementList(std::unique_ptr<Expr> stmt)
        : statement{std::move(stmt)} {}

    void interpet(void) override{}
    void visit(void) override  {
        statement->visit();
        if (next != nullptr) {
            next->visit();
        }
    }
    void add_child(std::unique_ptr<Expr> stmt) {
        if (next == nullptr) {
            next = std::make_unique<StatementList>(std::move(stmt));
        } else {
            next->add_child(std::move(stmt));
        }
    }
};

class Literal : public Expr {
    std::variant<int, bool, std::string> value;
public:
    Literal(std::variant<int, bool, std::string> value) : Expr{std::make_unique<Token>("", 0, token_type::UNKNOWN)}, value{value} { }

    Literal(std::unique_ptr<Token> &tok) : Expr(std::move(tok)) {
        switch (token->type) {
            case token_type::DIGIT:
                value = std::atoi(token->token.c_str());
                break;
            case token_type::STRING:
                value = token->token;
                break;
            default:
                std::cout << "Error line " << token->line <<
                    ": invalid token";
                break;
        }
    }

    virtual void interpet(void) override {}
    virtual void visit(void) override  {
        std::visit([](const auto &x) { std::cout << x << " "; }, value);
    }
};

class Bop : public Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    public:
        Bop(std::unique_ptr<Token> tok, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
            : Expr(std::move(tok)), left{std::move(left)}, right{std::move(right)} {}

        virtual void interpet(void) override {}
        virtual void visit(void) override  {
            std::cout << token->token << " ( ";
            left->visit();
            if (right)
                right->visit();
            std::cout << ") ";
        }
};

class VarInst : public Expr {
    enum token_type type;

    public:
        VarInst(std::unique_ptr<Token> tok, std::unique_ptr<Token> type) :
            Expr(std::move(tok)), type{type->type} {}

        virtual void interpet() {}
        virtual void visit () {
            std::cout << token->token << " ";
        }
};

class VarAssign : public Expr {

    public:
        VarAssign(std::unique_ptr<Token> tok) :
            Expr(std::move(tok)) {}

        virtual void interpet(void) override {}
        virtual void visit(void) override  {
            std::cout << token->token << " ";
        }
};

class If : public Expr {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> truthy;
    std::unique_ptr<Expr> falsy;

    public:
        If(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> truthy,
                std::unique_ptr<Expr> falsy) :
            Expr(), condition{std::move(condition)}, truthy{std::move(truthy)},
            falsy{std::move(falsy)} {}

        void interpet() override {}
        void visit() override {
            std::cout << "( IF ";
            condition->visit();
            std::cout << "(";
            truthy->visit();
            std::cout << ")";
            if (falsy) {
                std::cout << " ELSE (";
                falsy->visit();
                std::cout << ")";
            }
            std::cout << ")";
        }
};

#endif
