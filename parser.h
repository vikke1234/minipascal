#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "expr.h"
#include "symbols.h"
#include <memory>
#include <string_view>

struct Ast {
    std::string symbol;
    enum token_type type;
    std::vector<Ast> leaves;
};

class Parser {
    Lexer lexer;
    SymbolTable symbols;
public:
    Parser(std::string_view filename) : lexer(filename), symbols{} {}
    void parse_file();

private:
    // grammar translators

    /*
     * Parses a statement
     */
    std::unique_ptr<Expr> statement() {
        auto symbol = lexer.get_token();
        if (!lexer.is_reserved(symbol->token)) {
            std::cout << "Statement\n";
            std::unique_ptr<Expr> ident =
                std::make_unique<Ident>(std::move(symbol));

            match(":=");
            auto eq = lexer.get_token();
            std::unique_ptr<Expr> expr = expression();
            std::unique_ptr<Expr> assign = std::make_unique<Bop>(
                    std::move(eq), std::move(ident), std::move(expr));


            return assign;
        } else {
            std::cout << "reserved";
            switch(symbol->type) {
                default:
                    return nullptr;
            }
        }
    }

    /**
     * Fetches a terminal, e.g. digit or identifier
     */
    std::unique_ptr<Expr> terminal() {
        std::cout << "Term\n";
        auto symbol = lexer.peek_token();
        switch(symbol->type) {
            case token_type::IDENTIFIER: // fallthrough
            case token_type::DIGIT:      // fallthrough
            case token_type::LPARENTHESES:
                {
                    auto ident = factor();
                    return factor_tail(std::move(ident));
                }
            default:
                break;
        }
        return nullptr;
    }

    std::unique_ptr<Expr> term_tail(std::unique_ptr<Expr> expr) {
        std::cout << "Term tail\n";
        auto symbol = lexer.get_token();
        switch (symbol->type) {
            case token_type::ADDITION: // fallthrough
            case token_type::SUBTRACTION:
                return std::make_unique<Bop>(std::move(symbol), std::move(expr), terminal());
                break;

            case token_type::RPARENTHESES:
            case token_type::SEMICOLON:
                lexer.get_token();
                break;

            default:
                std::cout << "Parse error term_tail " << symbol->token << " \n";
                break;
        }
        return nullptr;
    }

    std::unique_ptr<Expr> factor() {
        std::cout << "Factor\n";
        auto symbol = lexer.get_token();
        switch(symbol->type) {
            case token_type::IDENTIFIER:
                return std::make_unique<Ident>(std::move(symbol));
            case token_type::DIGIT:
                return std::make_unique<Literal>(symbol);
            case token_type::LPARENTHESES:
                expression();
                break;
            default:
                std::cout << "Parse error in factoring\n";
                break;
        }
        return nullptr;
    }

    std::unique_ptr<Expr> factor_tail(std::unique_ptr<Expr> ident) {
        auto symbol = lexer.peek_token();
        switch (symbol->type) {
            case token_type::DIVISION: //fallthrough
            case token_type::MULTIPLICATION:
                {
                    lexer.get_token();
                    auto fac = factor();
                    return std::make_unique<Bop>(std::move(symbol), std::move(ident), factor_tail(std::move(fac)));
                }
                break;

            case token_type::RPARENTHESES:
            case token_type::SEMICOLON:
                lexer.get_token();
                break;

            default:
                break;
        }
        return ident;
    }

    std::unique_ptr<Expr> expression() {
        auto symbol = lexer.peek_token();
        switch (symbol->type) {
            case token_type::IDENTIFIER: // fallthrough
            case token_type::DIGIT:      // fallthrough
            case token_type::LPARENTHESES:
                {
                    auto left = terminal();
                    auto op = term_tail(std::move(left));
                    return op;
                }
                break;
            default:
                std::cout << "Parse error in expression expr, got token "
                    << symbol->token;
        }
        return nullptr;
    }

    bool match(std::string_view tok) {
        auto token = lexer.peek_token();
        if (token->token == tok.data()) {
            return true;
        }
        std::cout << "Parse error: expected " << tok <<
            " got " << token->token << "\n";
        return false;
    }

    Expr comparison(void);

};
#endif
