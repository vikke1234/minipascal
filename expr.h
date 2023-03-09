/**
 * This file contains definitions for all symbols that the parser needs.
 */
#ifndef EXPR_H
#define EXPR_H
#include <memory>
#include <string>
#include <variant>
#include <iostream>

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
    virtual void visit(void) const = 0;
    virtual void analyse() const = 0;
};

class StatementList : public Expr {
    std::unique_ptr<Expr> statement;
    std::unique_ptr<StatementList> next;

public:
    StatementList(std::unique_ptr<Expr> stmt)
        : statement{std::move(stmt)} {}

    void analyse() const override {};
    void interpet(void) override {}
    void visit(void) const override {
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

    const Expr *get_statement() {
        return statement.get();
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

    void analyse() const override {}
    virtual void interpet(void) override {}
    virtual void visit(void) const override  {
        std::visit([](const auto &x) { std::cout << x << " "; }, value);
    }
};

class Bop : public Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    public:
        Bop(std::unique_ptr<Token> tok, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
            : Expr(std::move(tok)), left{std::move(left)}, right{std::move(right)} {}

        void analyse() const override {

        }

        virtual void interpet(void) override {}
        virtual void visit(void) const override  {
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
        VarInst(std::unique_ptr<Token> tok, enum token_type type) :
            Expr(std::move(tok)), type{type} {}

        VarInst(std::unique_ptr<Token> tok, std::unique_ptr<Token> type) :
            Expr(std::move(tok)), type{type->type} {}

        void analyse() const override {};
        virtual void interpet() override {}
        virtual void visit(void) const override {
            std::cout << token->token << " ";
        }
};

class Var : public Expr {

    public:
        Var(std::unique_ptr<Token> tok) :
            Expr(std::move(tok)) {}

        void analyse() const override {};
        virtual void interpet(void) override {}
        virtual void visit(void) const override  {
            std::cout << token->token << " ";
        }

        Literal *get_value() {
            return nullptr;
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

        void analyse() const override {};
        void interpet() override {}
        void visit() const override {
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

class Range : public Expr {
    int start;
    int end;
    int current;

    public:
        Range(std::unique_ptr<Token> start, std::unique_ptr<Token> end) : Expr() {
            this->start = std::atoi(start->token.c_str());
            this->end = std::atoi(end->token.c_str());
        }

        int get_next() {
            return current++;
        }

        bool is_done() {
            return start <= end;
        }

        void analyse() const override {};
        void interpet() override {}
        void visit() const override {
            std::cout << start << ".." << end;
        }
};

class For : public Expr {
    std::unique_ptr<VarInst> var;
    std::unique_ptr<Range> range;
    std::unique_ptr<StatementList> loop;

    public:
        For(std::unique_ptr<Token> variable, std::unique_ptr<Range> range,
                std::unique_ptr<StatementList> statement_list) : Expr(), var{std::make_unique<VarInst>(std::move(variable), token_type::INT)},
                    range{std::move(range)}, loop{std::move(statement_list)} {}

        void analyse() const override {};
        void interpet() override {}
        void visit(void) const override {
            std::cout << "(FOR " << token->token << " ";
            range->visit();
            std::cout << " ( ";
            loop->visit();
            std::cout << ")";
        }
};

class Print : public Expr {
    std::unique_ptr<Operand> op;
    Print(std::unique_ptr<Operand> op) : Expr(), op{std::move(op)} {}
};

class Read : public Expr {
    std::unique_ptr<Operand> op;
    Read(std::unique_ptr<Operand> op) : Expr(), op{std::move(op)} {}
};

class Unary : public Operand {
    std::unique_ptr<Operand> op;


    public:
        Unary(std::unique_ptr<Operand> op) : Operand(), op {std::move(op)} {}

        virtual Literal *get_value() override {
            return nullptr;
        }

        virtual int get_type() override {
           return 1;
        }

        virtual void interpet(void) override {

        }

        virtual void visit(void) const override {
            std::cout << "( ! ";
            op->visit();
            std::cout << ") ";
        }

        virtual bool analyse() const override {
            return false;
        }
};
#endif
