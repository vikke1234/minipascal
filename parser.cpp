#include "parser.h"
#include "expr.h"
#include <iostream>
#include <memory>

void Parser::parse_file() {
    std::unique_ptr<StatementList> program;

    while (true) {
        std::unique_ptr<Token> token = lexer.peek_token();

        if (token->type == NO_SYMBOLS) {
            break;
        }
        std::unique_ptr<Expr> e = statement();

        if (program != nullptr) {
            program->add_child(std::move(e));
        } else {
            program = std::make_unique<StatementList>(std::move(e));
        }
    }
    program->visit();
}

