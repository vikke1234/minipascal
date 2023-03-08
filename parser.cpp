#include "parser.h"
#include "expr.h"
#include <iostream>
#include <memory>

std::unique_ptr<StatementList> Parser::parse_file() {
    std::unique_ptr<StatementList> program = statement_list(false);

    program->visit();
    return program;
}

std::unique_ptr<StatementList> Parser::statement_list(bool is_block) {
    std::unique_ptr<StatementList> program;

    while (true) {
        std::unique_ptr<Token> token = lexer.peek_token();

        if (token->type == token_type::NO_SYMBOLS ||
                (is_block && (token->type == token_type::END || token->type == token_type::ELSE))) {
            break;
        }

        std::unique_ptr<Expr> e = statement();

        if (program != nullptr) {
            program->add_child(std::move(e));
        } else {
            program = std::make_unique<StatementList>(std::move(e));
        }
    }

    return program;
}

std::unique_ptr<Expr> Parser::term_tail(std::unique_ptr<Expr> expr) {
    std::cout << "Term tail\n";
    auto symbol = lexer.peek_token();
    switch (symbol->type) {
        case token_type::EQ:       // fallthrough
        case token_type::LT:       // fallthrough
        case token_type::ADDITION: // fallthrough
        case token_type::SUBTRACTION:
            {
                lexer.get_token();
                std::unique_ptr<Expr> bop = std::make_unique<Bop>(std::move(symbol), std::move(expr), terminal());

                return term_tail(std::move(bop));
            }

        case token_type::DO:
        case token_type::SEMICOLON:
            break;

        default:
            std::cout << "Parse error term_tail " << type_to_str(symbol->type) << " \n";
            break;
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::cout << "Factor\n";
    auto symbol = lexer.peek_token();
    switch(symbol->type) {
        case token_type::IDENTIFIER:
            lexer.get_token();
            return std::make_unique<Var>(std::move(symbol));

        case token_type::DIGIT:
            lexer.get_token();
            return std::make_unique<Literal>(symbol);

        case token_type::LPARENTHESES:
            {
                lexer.get_token();
                auto expr = expression();
                match(token_type::RPARENTHESES);
                return expr;
            }

        default:
            std::cout << "Parse error in factoring\n";
            break;
    }
    return nullptr;
}
std::unique_ptr<Expr> Parser::factor_tail(std::unique_ptr<Expr> ident) {
    auto symbol = lexer.peek_token();
    switch (symbol->type) {
        case token_type::DIVISION: //fallthrough
        case token_type::AND: // fallthrough
        case token_type::MULTIPLICATION:
            {
                lexer.get_token();
                auto fac = factor();
                return std::make_unique<Bop>(std::move(symbol), std::move(ident), factor_tail(std::move(fac)));
            }
            break;

        default:
            break;
    }
    return ident;
}

std::unique_ptr<Expr> Parser::expression() {
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
std::unique_ptr<Expr> Parser::terminal() {
    auto symbol = lexer.peek_token();

    switch(symbol->type) {
        case token_type::IDENTIFIER: // fallthrough
        case token_type::DIGIT:      // fallthrough
        case token_type::LPARENTHESES:
            {
                auto ident = factor();
                auto tail = factor_tail(std::move(ident));
                return tail;
            }

        default:
            break;
    }
    return nullptr;
}

std::unique_ptr<Expr> Parser::var() {

    std::unique_ptr<Token> symbol = match(token_type::IDENTIFIER);
    match(token_type::TYPE_DELIM);
    std::unique_ptr<Token> type = lexer.get_token();
    std::unique_ptr<Token> has_assign = lexer.peek_token();
    std::unique_ptr<VarInst> variable = std::make_unique<VarInst>(std::move(symbol), std::move(type));

    std::unique_ptr<Bop> assign;

    if (has_assign->type == token_type::ASSIGN) {
        lexer.get_token();
        std::unique_ptr<Expr> init = expression();
        assign = std::make_unique<Bop>(std::move(has_assign),
                                       std::move(variable), std::move(init));
    } else {
        std::variant<int, bool, std::string> default_init;

        switch(type->type) {
            case token_type::INT:
                default_init = 0;
                break;

            case token_type::STRING:
                default_init = "";
                break;

            case token_type::BOOL:
                default_init = false;
                break;
            default:
                std::cout << "Invalid type\n";
        }

        auto as_tok = std::make_unique<Token>(":=", 0, token_type::ASSIGN);
        auto literal = std::make_unique<Literal>(default_init);
        assign = std::make_unique<Bop>(std::move(as_tok), std::move(variable), std::move(literal));
    }

    match(token_type::SEMICOLON);
    return assign;
}

std::unique_ptr<Expr> Parser::statement() {
    auto symbol = lexer.peek_token();

    if (!lexer.is_reserved(symbol->token)) {
        lexer.get_token();
        std::cout << "Statement\n";
        std::unique_ptr<Var> ident =
            std::make_unique<Var>(std::move(symbol));


        auto eq = match(token_type::ASSIGN);

        if (eq == nullptr) {
            return nullptr;
        }

        auto expr = expression();
        std::unique_ptr<Expr> assign = std::make_unique<Bop>(
                std::move(eq), std::move(ident), std::move(expr));

        match(token_type::SEMICOLON);
        return assign;
    } else {
        switch(symbol->type) {
            case token_type::VAR:
                lexer.get_token();
                return var();

            case token_type::IF:
                lexer.get_token();
                return if_stmt();
            case token_type::FOR:
                lexer.get_token();
                return for_loop();
            default:
                return nullptr;
        }
    }
}
