/**
 * This file contains definitions for all symbols that the parser needs.
 */
#ifndef EXPR_H
#define EXPR_H
#include <string>
#include <variant>

#include <lexer.h>


class Expr {
protected:
    std::unique_ptr<Token> token;
public:
    Expr(std::unique_ptr<Token> t) : token{std::move(t)} {}
    virtual ~Expr() = default;

    virtual void interpet(void) = 0;
    virtual void visit(void) = 0;
};

class Statement : public Expr {
    token_type type;
    Statement * next;

public:
    Statement(std::unique_ptr<Token> &t) : Expr(std::move(t)) { }
    virtual void interpet(void) {}
    virtual void visit(void) {}
};

/*
class StatementList : public Expr {
    Statement *stmt;

public:
    StatementList *next;

    StatementList(Statement *s) : stmt(s) {}
    void interpet(void){}
    void visit(void){}
};
*/

class Literal : public Expr {
    std::variant<int, std::string> value;
public:
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

    virtual void interpet(void) {}
    virtual void visit(void) {
        std::visit([](const auto &x) { std::cout << x << " "; }, value);
    }
};

class Bop : public Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    public:
        Bop(std::unique_ptr<Token> tok, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
            : Expr(std::move(tok)), left{std::move(left)}, right{std::move(right)} {}

        virtual void interpet(void) {}
        virtual void visit(void) {
            std::cout << token->token << " ( ";
            left->visit();
            right->visit();
            std::cout << ")\n";
        }
};

class Ident : public Expr {

    public:
        Ident(std::unique_ptr<Token> tok) :
            Expr(std::move(tok)) {}

        virtual void interpet(void) {}
        virtual void visit(void) {
            std::cout << token->token << " ";
        }
};

/*
class Type : public Expr {
    Token type;
public:
    Type(Token t) : type(t) { }
    void interpet(void){}
    void visit(void){}
};

class Var : public Expr {
public:
    void interpet(void){}
    void visit(void){}
};
*/

#endif
