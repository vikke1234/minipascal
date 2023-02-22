#include "parser.h"
#include "expr.h"
#include <iostream>

void Parser::parse_file() {
    while (true) {
        std::unique_ptr<Token> token = lexer.peek_token();

        if (token->type == NO_SYMBOLS) {
            break;
        }

        switch(token->type) {
            case token_type::IDENTIFIER:
                {
                    auto e = statement();
                    e->visit();
                }
                break;

            default:
                break;
        }
    }
}

