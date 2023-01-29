#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <string>
#include <string_view>

enum token_type {
    IDENTIFIER,
    COMMENT,
    OPERATOR,
    DIGIT,
    STRING,
    KEYWORD,
    PARENTHESES,
    TYPE_DELIM,
    DELIM,
    UNKNOWN,
    ASSIGN,
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
        line = 0ULL;
        current_char = EOF;
    }
    struct Token get_token(void);

private:
    using identifier = int (*)(int);

    std::string read_file(std::string_view filename);
    std::string parse(identifier f);
    bool is_assign(void);
    Lexer::comment_type is_comment(void);
    void skip_comment(Lexer::comment_type);
    void skip_wspace(void);
    bool is_keyword(void);
    char get_char(void);
    char peek_char(void);
};

#endif /* LEXER_H */
