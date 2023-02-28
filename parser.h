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

    /**
     * Fetches the tail if it exists, if not it will return the terminal
     *
     * @param expr - the terminal, the left side operand of the expression that
     *               is being built
     */
    std::unique_ptr<Expr> term_tail(std::unique_ptr<Expr> expr);

    /**
     * Fetches a digit/identifier, also makes sure parentheses are handled.
     */
    std::unique_ptr<Expr> factor();

    /**
     * Tries to factor the expression if needed, for example handles
     * parentheses and operator order.
     *
     * @param ident - left side of the expression (e.g. 1 * 3, the `1` will be
     * passed)
     */
    std::unique_ptr<Expr> factor_tail(std::unique_ptr<Expr> ident);

    /**
     * Parses an expression
     */
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
