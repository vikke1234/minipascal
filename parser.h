#ifndef PARSER_H
#define PARSER_H
#include "token.h"
#include "lexer.h"
#include "expr.h"
#include "symbols.h"
#include <memory>
#include <string_view>
#include <iostream>

class Parser {
    Lexer lexer;
public:
    Parser(std::string_view filename) : lexer(filename) {}
    std::unique_ptr<StatementList> parse_file();

private:
    // grammar translators

    /*
     * Parses a statement
     */
    std::unique_ptr<Expr> statement();

    std::unique_ptr<StatementList> statement_list(bool is_block);

    /**
     * Parses a var expression
     */
    std::unique_ptr<Expr> var();

    std::unique_ptr<Expr> print() {
        return nullptr;
    }

    std::unique_ptr<Expr> write() {
        return nullptr;
    }

    std::unique_ptr<Read> read_statement() {
        auto tok = match(token_type::IDENTIFIER);

        std::unique_ptr<Read> r = std::make_unique<Read>(
                std::make_unique<Var>(std::move(tok)));
        match(token_type::SEMICOLON);
        return r;
    }

    std::unique_ptr<Print> print_statement() {
        std::unique_ptr<Print> r = std::make_unique<Print>(expression());
        match(token_type::SEMICOLON);
        return r;
    }

    std::unique_ptr<For> for_loop() {
        std::unique_ptr<Token> identifier = match(token_type::IDENTIFIER);
        match(token_type::IN);

        auto start = expression();
        match(token_type::RANGE);
        auto end = expression();
        std::unique_ptr<Range> range =
            std::make_unique<Range>(std::move(start), std::move(end));

        match(token_type::DO);
        std::unique_ptr<StatementList> body = statement_list(true);
        match(token_type::END);
        return std::make_unique<For>(std::move(identifier), std::move(range), std::move(body));;
    }

    std::unique_ptr<If> if_stmt() {
        std::unique_ptr<Operand> condition = expression();
        match(token_type::DO);
        std::unique_ptr<StatementList> list = statement_list(true);
        std::unique_ptr<StatementList> else_stmt = nullptr;

        auto token = lexer.peek_token();
        switch(token->type) {
            case token_type::ELSE:
                lexer.get_token();
                else_stmt = statement_list(true);

                // fallthrough
            case token_type::END:
                match(token_type::END); // expect an end if coming from else
                break;

            default:
                break;
        }

        auto if_ = std::make_unique<If>(std::move(condition),
                std::move(list), std::move(else_stmt));
        return if_;
    }

    /**
     * Fetches a terminal, e.g. digit or identifier
     */
    std::unique_ptr<Operand> terminal();

    /**
     * Fetches the tail if it exists, if not it will return the terminal
     *
     * @param expr - the terminal, the left side operand of the expression that
     *               is being built
     */
    std::unique_ptr<Operand> term_tail(std::unique_ptr<Operand> expr);

    /**
     * Fetches a digit/identifier, also makes sure parentheses are handled.
     */
    std::unique_ptr<Operand> factor();

    /**
     * Tries to factor the expression if needed, for example handles
     * parentheses and operator order.
     *
     * @param ident - left side of the expression (e.g. 1 * 3, the `1` will be
     * passed)
     */
    std::unique_ptr<Operand> factor_tail(std::unique_ptr<Operand> ident);

    /**
     * Parses an expression
     */
    std::unique_ptr<Operand> expression();

    std::unique_ptr<Token> match(token_type expected) {
        auto token = lexer.peek_token();
        if (token->type == expected) {
            return lexer.get_token();
        }
        std::cout << "Parse error: expected " << type_to_str(expected) <<
            " got " << type_to_str(token->type) << "(" << token->token << ")\n";
        std::exit(1);
    }
};
#endif
