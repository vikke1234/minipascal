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
    Expr(const Token &t) : token(std::make_unique<Token>(t)) {}
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
        Operand(const Token &t) : Expr(t) {}
        Operand(const Token &&t) : Expr(t) {}
        Operand() = default;
        /**
         * Gets the value of a variable/literal/operation.
         */
        virtual Literal *get_value() = 0;
        virtual std::unique_ptr<Literal> own_value() { return nullptr; };
        virtual bool truthy() = 0;

        /**
         * Gets the type of a variable/literal/operation.
         */
        virtual int get_type() = 0;
        virtual Literal operator+(Operand &l) = 0;
        virtual Literal operator-(Operand &l) = 0;
        virtual Literal operator*(Operand &l) = 0;
        virtual Literal operator/(Operand &l) = 0;
        virtual Literal operator&&(Operand &l) = 0;
        virtual Literal operator==(Operand &l) = 0;
        virtual Literal operator<(Operand &l)= 0;
        virtual Literal operator!() = 0;
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

    bool analyse() const override {
        bool error = statement->analyse();

        if (next != nullptr) {
            error |= next->analyse();
        }
        return error;
    }

    void interpet(void) override {
        statement->interpet();
        if (next != nullptr) {
            next->interpet();
        }
    }

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

    StatementList *get_next() const {
        return next.get();
    }

    Expr *get_statement() const {
        return statement.get();
    }
};

/**
 * A literal, a digit or string, since booleans are not part of the spec, they
 * can not exist as a literal.
 */
class Literal : public Operand {

public:
    std::variant<int, std::string, bool> value;
    Literal(std::variant<int, std::string, bool> value) : Operand(), value{value} { }
    Literal(const Literal &l)  : Operand(), value{l.value} {}
    Literal(const Literal &&l) : Operand(), value{l.value} {}

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

    bool truthy() override {
        if (std::holds_alternative<int>(this->value)) {
            return !!std::get<int>(this->value);
        } else if(std::holds_alternative<bool>(this->value)) {
            return !!std::get<bool>(this->value);
        } else {
            return std::get<std::string>(this->value) != "";
        }
    }

    virtual int get_type() override {
        return value.index();
    }

    Literal operator+(Operand &l) override {
        if (std::holds_alternative<int>(this->value) &&
                std::holds_alternative<int>(l.get_value()->value)) {
            return Literal{std::get<int>(this->value) + std::get<int>(l.get_value()->value)};
        } else if (std::holds_alternative<std::string>(this->value) &&
                std::holds_alternative<std::string>(l.get_value()->value)) {

            return Literal{std::get<std::string>(this->value) + std::get<std::string>(l.get_value()->value)};
        }
        std::cout << "Error invalid types in + operation";
        std::exit(1);
    }

    Literal operator=(Literal &l) {
        if (this->value.index() == l.value.index()) {
            this->value = l.value;
            return *this;
        }
        std::cout << "Conflicting types in assignment";
        std::exit(1);
    }

    Literal operator=(std::variant<int, std::string, bool> value) {
        this->value = value;
        return *this;
    }

    Literal operator-(Operand &l) override {
        if (std::holds_alternative<int>(this->value) && std::holds_alternative<int>(l.get_value()->value)) {
            return Literal{std::get<int>(this->value) - std::get<int>(l.get_value()->value)};
        }

        std::cout << "Error invalid types in - operation";
        std::exit(1);
    }

    Literal operator*(Operand &l) override {
        if (std::holds_alternative<int>(this->value) && std::holds_alternative<int>(l.get_value()->value)) {
            return Literal{std::get<int>(this->value) * std::get<int>(l.get_value()->value)};
        }

        std::cout << "Error invalid types in - operation";
        std::exit(1);
    }

    Literal operator/(Operand &l) override {
        if (std::holds_alternative<int>(this->value) && std::holds_alternative<int>(l.get_value()->value)) {
            return Literal{std::get<int>(this->value) / std::get<int>(l.get_value()->value)};
        }

        std::cout << "Error invalid types in / operation";
        std::exit(1);
    }

    Literal operator&&(Operand &l) override {
        if (std::holds_alternative<bool>(this->value) && std::holds_alternative<bool>(l.get_value()->value)) {
            return Literal{std::get<bool>(this->value) && std::get<bool>(l.get_value()->value)};
        }

        std::cout << "Error invalid types in & operation";
        std::exit(1);
    }

    Literal operator==(Operand &l) override {
        if (this->value.index() == l.get_value()->value.index()) {
            switch(this->value.index()) {
                case 0: // int
                    return Literal{std::get<int>(this->value) == std::get<int>(l.get_value()->value)};
                case 1: // std::string
                    return Literal{std::get<std::string>(this->value) == std::get<std::string>(l.get_value()->value)};
                case 2:
                    return Literal{std::get<bool>(this->value) == std::get<bool>(l.get_value()->value)};
            }
        }

        std::cout << "Error invalid types in = operation";
        std::exit(1);
    }

    Literal operator<(Operand &l) override {
        if (this->value.index() == l.get_value()->value.index()) {
            switch(this->value.index()) {
                case 0: // int
                    return Literal{std::get<int>(this->value) < std::get<int>(l.get_value()->value)};
                case 1: // std::string
                    return Literal{std::get<std::string>(this->value) < std::get<std::string>(l.get_value()->value)};
                case 2:
                    return Literal{std::get<bool>(this->value) < std::get<bool>(l.get_value()->value)};
            }
        }

        std::cout << "Error invalid types in < operation";
        std::exit(1);
    }

    Literal operator!() override {
        if (std::holds_alternative<bool>(this->value)) {
            return Literal{!std::get<bool>(this->value)};
        }

        std::cout << "Error invalid types in & operation";
        std::exit(1);

    }
};

/**
 * A reference to a variable.
 */
class Var : public Operand {

    public:
        Var(std::unique_ptr<Token> tok) :
            Operand(std::move(tok)) {}
        Var(const Var &var) :
            Operand(*var.token) {}

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

        bool truthy() override {
            return symbol_table.get_symbol(token->token)->truthy();
        }

        int get_type() override {
            return symbol_table.get_symbol(token->token)->get_type();
        }

        void set_value(Literal *literal) {
            symbol_table.set_value(token->token, literal);
        }

        void set_value(std::variant<int, std::string, bool> value) {
            symbol_table.set_value(token->token, value);
        }


        Literal operator+(Operand &l) override {
            Literal * ptr = symbol_table.get_symbol(token->token);
            return *ptr + *l.get_value();
        }

        Literal operator-(Operand &l) override {
            Literal * ptr = symbol_table.get_symbol(token->token);
            return *ptr - *l.get_value();
        }

        Literal operator*(Operand &l) override {
            Literal * ptr = symbol_table.get_symbol(token->token);
            return *ptr * *l.get_value();
        }

        Literal operator/(Operand &l) override {
            Literal * ptr = symbol_table.get_symbol(token->token);
            return *ptr * *l.get_value();
        }

        Literal operator&&(Operand &l) override {
            Literal * ptr = symbol_table.get_symbol(token->token);
            return *ptr && *l.get_value();
        }

        Literal operator==(Operand &l) override {
            Literal * ptr = symbol_table.get_symbol(token->token);
            return *ptr == *l.get_value();
        }

        Literal operator!() override {
            Literal * ptr = symbol_table.get_symbol(token->token);
            return !*ptr;
        }

        Literal operator<(Operand &l) override {
            Literal * ptr = symbol_table.get_symbol(token->token);
            return *ptr < *l.get_value();
        }
};

/**
 * Initialization of a variable.
 */
class VarInst : public Var {
    int type;

    public:
        VarInst(std::unique_ptr<Token> tok, enum token_type type) :
            Var(std::move(tok)), type{type - token_type::INT} {}

        VarInst(std::unique_ptr<Token> tok, std::unique_ptr<Token> type) :
            Var(std::move(tok)), type{type->type - token_type::INT} {}

        int get_type() override {
            return symbol_table.get_symbol(token->token)->get_type();
        }

        bool analyse() const override {
            bool succeeded = symbol_table.add_symbol(token->token, type);

            if(!succeeded) {
                std::cout << "Error token variable " << token->token
                    << " already initialized";
                return true;
            }
            return false;
        }

        // handled by Bop
        virtual void interpet() override {}
        virtual void visit(void) const override {
            std::cout << token->token << " ";
        }

        Literal operator+(Operand &) override {
            std::cout << "invalid operation\n";
            std::exit(1);
        }

        Literal operator-(Operand &) override {
            std::cout << "invalid operation\n";
            std::exit(1);
        }

        Literal operator*(Operand &) override {
            std::cout << "invalid operation\n";
            std::exit(1);
        }

        Literal operator/(Operand &) override {
            std::cout << "invalid operation\n";
            std::exit(1);
        }

        Literal operator&&(Operand &) override {
            std::cout << "invalid operation\n";
            std::exit(1);
        }

        Literal operator==(Operand &) override {
            std::cout << "invalid operation\n";
            std::exit(1);
        }

        Literal operator<(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }

        Literal operator!() override {
            std::cout << "invalid operation\n";
            std::exit(1);
        }
};


/**
 * A binary operation, 1 + 1 for example
 */
class Bop : public Operand {
    std::unique_ptr<Operand> left;
    std::unique_ptr<Operand> right;
    std::unique_ptr<Literal> evaluated = nullptr;

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

            if(has_error) {
                std::cout << "Semantical error in bop\n";
            }


            return has_error;
        }

        /**
         * Gets the value of the binary operation, has to be free'd when done.
         */
        virtual Literal *get_value() override {
            if (evaluated) {
                // use cached value
                return evaluated.get();
            }

            switch(token->type) {
                case token_type::ADDITION:
                    evaluated = std::make_unique<Literal>(*left + *right->get_value());
                    break;

                case token_type::SUBTRACTION:
                    evaluated = std::make_unique<Literal>(*left - *right->get_value());
                    break;

                case token_type::MULTIPLICATION:
                    evaluated = std::make_unique<Literal>(*left * *right->get_value());
                    break;

                case token_type::DIVISION:
                    evaluated = std::make_unique<Literal>(*left / *right->get_value());
                    break;

                case token_type::LT:
                    evaluated = std::make_unique<Literal>(*left < *right->get_value());
                    break;

                case token_type::EQ:
                    evaluated = std::make_unique<Literal>(*left == *right->get_value());
                    break;

                case token_type::AND:
                    evaluated = std::make_unique<Literal>(*left && *right->get_value());
                    break;

                case token_type::ASSIGN:
                    {
                        auto var = dynamic_cast<Var&>(*left);
                        var.set_value(right->get_value());
                    }
                    break;
                default:
                    std::cout << "Invalid operation " << token->token;
                    break;
            }
            return evaluated.get();
        }

        bool truthy() override {
            return get_value()->truthy();
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

        virtual void interpet(void) override {
            this->get_value();
        }

        virtual void visit(void) const override  {
            std::cout << token->token << " ( ";
            left->visit();
            if (right)
                right->visit();
            std::cout << ") ";
        }

        virtual Literal operator+(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }
        virtual Literal operator-(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }
        virtual Literal operator*(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }
        virtual Literal operator/(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }
        virtual Literal operator&&(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }
        virtual Literal operator==(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }
        virtual Literal operator<(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }
        virtual Literal operator!() override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }
    protected:
        bool is_boolean() {
            return token->type == token_type::LT || token->type == token_type::EQ;
        }
};



/**
 * If statement node
 */
class If : public Expr {
    std::unique_ptr<Operand> condition;
    std::unique_ptr<StatementList> truthy;
    std::unique_ptr<StatementList> falsy;

    public:
        If(std::unique_ptr<Operand> condition, std::unique_ptr<StatementList> truthy,
                std::unique_ptr<StatementList> falsy) :
            Expr(), condition{std::move(condition)}, truthy{std::move(truthy)},
            falsy{std::move(falsy)} {}

        bool analyse() const override {
            bool error = condition->analyse() || truthy->analyse();
            if (falsy != nullptr) {
                error |= falsy->analyse();
            }
            return error;
        }

        void interpet() override {
            if(condition->truthy()) {
                truthy->interpet();
            } else {
                if (falsy != nullptr) {
                    falsy->interpet();
                }
            }
        }

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
    std::unique_ptr<Operand> start;
    std::unique_ptr<Operand> end;
    int current;

    public:
        Range(std::unique_ptr<Operand> start, std::unique_ptr<Operand> end) :
            Expr(), start{std::move(start)}, end{std::move(end)}, current{0} {}

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
            return !(current <= (std::get<int>(end->get_value()->value)+1));
        }

        bool analyse() const override {
           bool error = start->get_type() || end->get_type();
           if (error) {
               std::cout << "Error range contains non integers\n";
               return true;
           }
           return false;
        }

        void interpet() override {}
        void visit() const override {
            start->visit();
            std::cout << "..";
            end->visit();
        }
};

/**
 * For loop node in the AST.
 */
class For : public Expr {
    std::unique_ptr<Var> var;
    std::unique_ptr<Range> range;
    std::unique_ptr<StatementList> loop;

    public:
        For(std::unique_ptr<Token> variable, std::unique_ptr<Range> range,
                std::unique_ptr<StatementList> statement_list) : Expr(), var{std::make_unique<Var>(std::move(variable))},
                    range{std::move(range)}, loop{std::move(statement_list)} {}

        bool analyse() const override {
            return var->analyse() || loop->analyse() || range->analyse();
        }

        void interpet() override {
            while(!range->is_done()) {
                loop->interpet();
                var->set_value(range->get_next());
            }
        }

        void visit(void) const override {
            std::cout << "(FOR ";
            var->visit();
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
    std::unique_ptr<Operand> expr;

public:
    Print(std::unique_ptr<Operand> expr) : Expr(), expr{std::move(expr)} {}

    void interpet(void) override {
        std::visit([](const auto &x) { std::cout << x << "\n"; }, expr->get_value()->value);
    }
    void visit(void) const override {
        std::cout << "( PRINT ";
        expr->visit();
        std::cout << ") ";
    }
    bool analyse() const override { return expr->analyse(); }
};

/**
 * Read node for standard input in the AST.
 */
class Read : public Expr {
    std::unique_ptr<Var> var;

public:
    Read(std::unique_ptr<Var> op) : Expr(), var{std::move(op)} {}

    void interpet(void) override {
        switch(var->get_type()) {
            case 0:
                {
                    int val;
                    std::cin >> val;
                    var->set_value(val);
                }
                break;
            case 1:
                {
                    std::string str;
                    std::cin >> str;
                    var->set_value(str);
                }
                break;

            case 2:
                {
                    bool b;
                    std::cin >> b;
                    var->set_value(b);
                }
                break;
        }
    }

    void visit(void) const override {
        std::cout << "( READ ";
        var->visit();
        std::cout << ") ";
    }

    bool analyse() const override {
        return var->analyse();
    }
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

        bool truthy() override {
            return false;
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

        Literal operator+(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }

        Literal operator-(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }

        Literal operator*(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }

        Literal operator/(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }

        Literal operator&&(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }

        Literal operator==(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }

        Literal operator<(Operand &) override {
            std::cout << "Invalid operation\n";
            std::exit(1);
        }

        Literal operator!() override {
            return !*this->op;
        }
};
#endif
