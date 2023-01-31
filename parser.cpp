#include "parser.h"
#include <iostream>

void Parser::parse_file() {
    while (true) {
        Token token = lexer.get_token();
        std::cout << token.token << "\n";
        if (token.type == NO_SYMBOLS) {
            break;
        }

        switch(token.type) {
            case token_type::IF:
                break;

            case token_type::FOR:
                break;

            default:
                break;
        }
    }

}

void Parser::match(enum token_type type) {
    std::unordered_map<enum token_type, std::pair<char, char>> matchers = {
        {token_type::PARENTHESES, {'(', ')'}}
    };

    if (matchers.find(type) != matchers.end()) {
        lexer.get_token();
    }
}
