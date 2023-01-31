#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include <string_view>

struct Ast {
    std::string symbol;
    enum token_type type;
    std::vector<Ast> leaves;
};


class Expr {
public:
    Expr() {

    }
};

class Parser {
    Lexer lexer;
public:
    Parser(std::string_view filename) : lexer(filename) {}
    void parse_file();
    void match(enum token_type type);

};
#endif
