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
    /**
     * Print the AST prettily
     */
    virtual void visit(void) const = 0;

    /**
     * Does analysis on the current statement.
     */
    virtual bool analyse() const = 0;
};

class Operand : public Expr {
    public:

        Operand(std::unique_ptr<Token> t) : Expr(std::move(t)) {}
        Operand() = default;
        /**
         * Gets the value of a variable/literal/operation.
         */
        virtual Literal *get_value() = 0;

        /**
         * Gets the type of a variable/literal/operation.
         */
        virtual int get_type() = 0;
};

/**
 * The core of the AST, it will have the pointer to the current statement and
 * to the next.
 *
 * Example: X := 1;
 *     :=
 *   X    1
 */
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

/**
 * A literal, a digit or string, since booleans are not part of the spec, they
 * can not exist as a literal.
 */
class Literal : public Operand {
    std::variant<int, std::string, bool> value;

public:
    Literal(std::variant<int, std::string, bool> value) : Operand(), value{value} { }

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


/**
 * A binary operation, 1 + 1 for example
 */
class Bop : public Operand {
    std::unique_ptr<Operand> left;
    std::unique_ptr<Operand> right;

    public:
        /**
         * @param tok - which type of operation it is.
         * @param left - left side of the operation.
         * @param right - the right side of the operation.
         */
        Bop(std::unique_ptr<Token> tok, std::unique_ptr<Operand> left, std::unique_ptr<Operand> right)
            : Operand(std::move(tok)), left{std::move(left)}, right{std::move(right)} {}


        bool analyse() const override {
            bool has_error = left->analyse();
            has_error |= right->analyse();


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

/**
 * Initialization of a variable.
 */
class VarInst : public Operand {
    int type;

    public:
        VarInst(std::unique_ptr<Token> tok, enum token_type type) :
            Operand(std::move(tok)), type{type - token_type::INT} {}

        VarInst(std::unique_ptr<Token> tok, std::unique_ptr<Token> type) :
            Operand(std::move(tok)), type{type->type - token_type::INT} {}

        Literal *get_value() override {
            return 0;
        }

        int get_type() override {
            return symbol_table.get_symbol(token->token)->get_type();
        }

        bool analyse() const override {
            std::cout << "Analysing varinst\n";
            bool succeeded = symbol_table.add_symbol(token->token, type);
            if(!succeeded) {
                std::cout << "Error token variable " << token->token
                    << " already initialized";
                return false;
            }
            return true;
        }

        virtual void interpet() override {}
        virtual void visit(void) const override {
            std::cout << token->token << " ";
        }
};

/**
 * A reference to a variable.
 */
class Var : public Operand {

    public:
        Var(std::unique_ptr<Token> tok) :
            Operand(std::move(tok)) {}

        bool analyse() const override {
            if (!symbol_table.exists(token->token)) {
                std::cout << "Error no variable named " << token->token << "\n";
                return true;
            }
            return false;
        }

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

/**
 * If statement node
 */
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

/**
 * Range node for the AST.
 */
class Range : public Expr {
    int start;
    int end;
    int current;

    public:
        Range(std::unique_ptr<Token> start, std::unique_ptr<Token> end) : Expr() {
            this->start = std::atoi(start->token.c_str());
            this->end = std::atoi(end->token.c_str());
        }

        /**
         * Gets the next number for the range, needs to be manually checked
         * that it does not go out of range.
         */
        int get_next() {
            return current++;
        }

        /**
         * Checks if all numbers from the range is consumed.
         */
        bool is_done() {
            return start <= end;
        }

        bool analyse() const override { return false; }
        void interpet() override {}
        void visit() const override {
            std::cout << start << ".." << end;
        }
};

/**
 * For loop node in the AST.
 */
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

/**
 * Print node in the AST.
 */
class Print : public Expr {
    std::unique_ptr<Operand> op;
    Print(std::unique_ptr<Operand> op) : Expr(), op{std::move(op)} {}
};

/**
 * Read node for standard input in the AST.
 */
class Read : public Expr {
    std::unique_ptr<Operand> op;
    Read(std::unique_ptr<Operand> op) : Expr(), op{std::move(op)} {}
};

/**
 * Unary operation in the AST, needs to be handled differently from a normal
 * BOP.
 */
class Unary : public Operand {
    std::unique_ptr<Operand> op;


    public:
        Unary(std::unique_ptr<Operand> op) : Operand(), op {std::move(op)} {}

        virtual Literal *get_value() override {
            return nullptr;
        }

        virtual int get_type() override {
           return 2;
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
