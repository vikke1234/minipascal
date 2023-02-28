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
    std::unique_ptr<Expr> statement();
    /**
     * Parses a var expression
     */
    std::unique_ptr<Expr> var();

    /**
     * Fetches a terminal, e.g. digit or identifier
     */
    std::unique_ptr<Expr> terminal();
    std::unique_ptr<Expr> term_tail(std::unique_ptr<Expr> expr);
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> factor_tail(std::unique_ptr<Expr> ident);
    std::unique_ptr<Expr> expression();

    std::unique_ptr<Token> match(token_type expected) {
        auto token = lexer.peek_token();
        if (token->type == expected) {
            return lexer.get_token();
        }
        std::cout << "Parse error: expected " << type_to_str(expected) <<
            " got " << type_to_str(token->type) << "(" << token->token << ")\n";
        return nullptr;
    }

    Expr comparison(void);

};
#endif
