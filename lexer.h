#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

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
    PARENTHESES,
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

struct Token {
    std::size_t line;
    std::string token;
    enum token_type type;
};

class Lexer {
    std::size_t length;
    std::size_t index;
    std::size_t line;
    std::string content;
    std::unordered_map<std::string, enum token_type> reserved;
    char current_char;

    enum comment_type {
        NONE,
        CPP_COMMENT,
        C_COMMENT,
        C_COMMENT_END
    };

public:
    Lexer(std::string_view filename) {
        content = read_file(filename);
        std::cout << content;
        length = content.length();
        index = 0ULL;
        line = 1ULL;
        current_char = EOF;
        reserved = {
                {"!", NOT},  {"+", ADDITION}, {"-", SUBTRACTION},
                {"&", AND}, {"*", MULTIPLICATION}, {"<", LT}, {"=", EQ},
                {":=", ASSIGN}, {"\"", STRING}, {"(", PARENTHESES},
                {";", SEMICOLON}, {"..", RANGE},

                {"var", VAR},
                {"for", FOR},
                {"end", END},
                {"in", IN},
                {"do", DO},
                {"read", READ},
                {"print", PRINT},
                {"int", INT},
                {"string", STRING_TYPE},
                {"bool", BOOL},
                {"assert", ASSERT},
                {"if", IF},
                {"else", ELSE}
            };
    }
    struct Token get_token(void);

private:
    using identifier = int (*)(int);

    std::string read_file(std::string_view filename);
    std::string parse(identifier f);

    Lexer::comment_type is_comment(void);
    void skip_comment(Lexer::comment_type);
    void skip_wspace(void);

    std::string get_string(void);
    char get_char(void);
    char peek_char(void);

    enum token_type get_token_type(std::string current);
};

#endif /* LEXER_H */
