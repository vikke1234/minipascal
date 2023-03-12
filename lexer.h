#ifndef LEXER_H
#define LEXER_H

#include <iosfwd>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "token.h"


class Lexer {
    std::string content;
    std::size_t length;
    std::size_t index;
    std::size_t line;
    char current_char;
    Token previous;

    /**
     * Contains a map of each of the reserved keywords/symbols for the
     * minipascal language.
     */
    const std::unordered_map<std::string, enum token_type> reserved;

    enum comment_type {
        NONE,
        CPP_COMMENT,
        C_COMMENT,
        C_COMMENT_END
    };

public:
    Lexer(std::string_view filename) :
        content(read_file(filename)), length(content.length()), index(0ULL),
        line(1ULL), current_char(EOF), previous{"", 0, NO_SYMBOLS}, reserved{
                {"!", NOT},  {"+", ADDITION}, {"-", SUBTRACTION},
                {"&", AND}, {"*", MULTIPLICATION}, {"/", DIVISION}, {"<", LT}, {"=", EQ},
                {":=", ASSIGN}, {"\"", STRING}, {"(", LPARENTHESES},
                {")", RPARENTHESES},
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
            }
    {}

    /**
     * Gets the next token
     *
     * @param consume - whether to consume the returned token or not.
     *
     * @return The next token from the file.
     */
    std::unique_ptr<Token> get_token(bool consume = true);

    /**
     * Skips all whitespace and returns the next token,
     * it does not consume the token.
     *
     * @return The next token from the file.
     */
    std::unique_ptr<Token> peek_token(void);
    bool is_reserved(std::string_view lexeme) {
        return reserved.find(lexeme.data()) != reserved.end();
    }

private:
    using identifier = int (*)(int);

    std::string read_file(std::string_view filename);
    std::string parse(identifier f);

    /**
     * Checks if the the current character is a comment
     */
    Lexer::comment_type is_comment(void);

    /**
     * Goes forward in the file until a comment delimiter is found.
     */
    void skip_comment(Lexer::comment_type);

    /**
     * Goes forward in the file until non-whitespace is found.
     */
    void skip_wspace(void);

    /**
     * Gets a string literal, does not include the qoutes.
     */
    std::string get_string(void);
    char interpret_escape(char c);
    std::string parse_octal();
    std::string parse_hex();
    char get_char(void);
    char peek_char(void);

    enum token_type get_token_type(std::string current);
};

#endif /* LEXER_H */
