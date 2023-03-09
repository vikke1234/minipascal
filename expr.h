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
#include "symbols.h"
#include "lexer.h"

class Literal;


class Expr {
protected:
    std::unique_ptr<Token> token;
public:
    Expr(std::unique_ptr<Token> t) : token{std::move(t)} {}
    Expr() = default;
    virtual ~Expr() = default;

    virtual void interpet(void) = 0;
    virtual void visit(void) const = 0;
    virtual bool analyse() const = 0;
};

class Operand : public Expr {
    public:

        Operand(std::unique_ptr<Token> t) : Expr(std::move(t)) {}
        Operand() = default;
        virtual Literal *get_value() = 0;
        virtual int get_type() = 0;
};

class StatementList : public Expr {
    std::unique_ptr<Expr> statement;
    std::unique_ptr<StatementList> next;

public:
    StatementList(std::unique_ptr<Expr> stmt)
        : statement{std::move(stmt)} {}

    bool analyse() const override { return false; }
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

    const StatementList *get_next() const {
        return next.get();
    }

    const Expr *get_statement() const {
        return statement.get();
    }
};

class Literal : public Operand {
    std::variant<int, bool, std::string> value;

public:
    Literal(std::variant<int, bool, std::string> value) : Operand(), value{value} { }

    Literal(std::unique_ptr<Token> &tok) : Operand(std::move(tok)) {
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

    bool analyse() const override { return false; }
    virtual void interpet(void) override {}
    virtual void visit(void) const override  {
        std::visit([](const auto &x) { std::cout << x << " "; }, value);
    }
    virtual Literal *get_value() override {
        return this;
    }
    virtual int get_type() override {
        return value.index();
    }
};

class Bop : public Operand {
    std::unique_ptr<Operand> left;
    std::unique_ptr<Operand> right;

    public:
        Bop(std::unique_ptr<Token> tok, std::unique_ptr<Operand> left, std::unique_ptr<Operand> right)
            : Operand(std::move(tok)), left{std::move(left)}, right{std::move(right)} {}


        bool analyse() const override {
            bool has_error = left->get_type() != right->get_type();

            return has_error;
        }

        virtual Literal *get_value() override {
            return nullptr;
        }

        virtual int get_type() override {
            int r = right->get_type();
            int l = left->get_type();
            if (l != r) {
                std::cout << "Error: incompatible types line " << token->line << "\n";
                std::exit(1);
            } else {
                if (token->type == token_type::EQ || token->type == token_type::LT){
                    return 1;
                }
            }
            return l;
        }

        virtual void interpet(void) override {}
        virtual void visit(void) const override  {
            std::cout << token->token << " ( ";
            left->visit();
            if (right)
                right->visit();
            std::cout << ") ";
        }

    protected:
        bool is_boolean() {
            return token->type == token_type::LT || token->type == token_type::EQ;
        }
};

class VarInst : public Operand {
    enum token_type type;

    public:
        VarInst(std::unique_ptr<Token> tok, enum token_type type) :
            Operand(std::move(tok)), type{type} {}

        VarInst(std::unique_ptr<Token> tok, std::unique_ptr<Token> type) :
            Operand(std::move(tok)), type{type->type} {}

        Literal *get_value() override {
            return 0;
        }

        int get_type() override {
            return 0;
        }

        bool analyse() const override { return false; }

        virtual void interpet() override {}
        virtual void visit(void) const override {
            std::cout << token->token << " ";
        }
};

class Var : public Operand {

    public:
        Var(std::unique_ptr<Token> tok) :
            Operand(std::move(tok)) {}

        bool analyse() const override { return false; }
        virtual void interpet(void) override {}
        virtual void visit(void) const override  {
            std::cout << token->token << " ";
        }

        Literal *get_value() override {
            return symbol_table.get_symbol(token->token);
        }

        int get_type() override {
            return symbol_table.get_symbol(token->token)->get_type();
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

        bool analyse() const override { return false; }
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

        bool analyse() const override { return false; }
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

        bool analyse() const override { return false; }
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
