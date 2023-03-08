#ifndef TOKEN_H
#define TOKEN_H
#include <string>



enum token_type {
    IDENTIFIER,

    // operators
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    LT,
    NOT,
    EQ,
    AND,

    // symbols
    LPARENTHESES,
    RPARENTHESES,
    TYPE_DELIM,
    SEMICOLON,
    ASSIGN,
    RANGE,

    // literals
    DIGIT,
    STRING,

    // Keywords
    VAR,
    FOR,
    END,
    IN,
    DO,
    READ,
    PRINT,
    INT,
    STRING_TYPE,
    BOOL,
    ASSERT,
    IF,
    ELSE,

    // errors (essientially)
    UNKNOWN,
    NO_SYMBOLS
};

extern std::string type_to_str(token_type type);

struct Token {
    std::string token;
    std::size_t line;
    enum token_type type;

    Token(std::string token, std::size_t line, enum token_type type) :
        token{std::move(token)}, line{line}, type{type} {}
};

#endif // TOKEN_H
