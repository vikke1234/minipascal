#include <cctype>
#include <fstream>
#include <string_view>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "lexer.h"

/**
 * Parses the current word or number, identifies which by the identifier
 * function `f`
 *
 * @param f       - identifier function (isalpha, isdigit)
 */
std::string Lexer::parse(identifier f) {

    std::string word = std::string(1, current_char);

    while(true) {
        char c = peek_char();

        if (!f(c) || c == EOF) {
            break;
        }
        word.push_back(get_char());
    }
    return word;
}


/**
 * Reads all of the content in a file.
 *
 * @param filename - name of the file
 *
 * @return a string that contains all of the content in the file
 */
std::string Lexer::read_file(std::string_view filename) {
    constexpr std::size_t read_size = std::size_t(4096);
    std::ifstream file(filename.data(), std::ios::in);
    file.exceptions(std::ios::badbit);

    std::string out = std::string();
    std::string buffer = std::string(read_size, '\0');

    while (file.read(&buffer[0], read_size)) {
        out.append(buffer, 0, file.gcount());
    }
    out.append(buffer, 0, file.gcount());
    return out;
}

std::unique_ptr<Token> Lexer::get_token(bool consume) {
    std::size_t start = 0ULL;

    std::string current = std::string();
    enum token_type type;

    char c = get_char();
    // This is done here because if done within get_char it does not know the
    // context which becomes an issue
    if (std::isspace(c)) {
        skip_wspace();
        c = get_char();
    }

    if (!consume) {
        start = index - 1;
    }

    if (c == EOF) {
        return std::make_unique<Token>("", 0, NO_SYMBOLS);
    }

    if(std::isalpha(c)) {
        current = parse(isalnum);
        type = get_token_type(current);
    } else if(std::isdigit(c)) {
        current = parse(isdigit);
        type = DIGIT;
    } else {
        current = std::string(1, c);
        type = get_token_type(current);

        if (type == ASSIGN || type == RANGE) {
            current.push_back(get_char());
        } else if(type == STRING) {
            current = get_string();
        }
    }

    if (!consume) {
        index = start;
    }

    std::unique_ptr<Token> t = std::make_unique<Token>(current, line, type);
    return t;
}

std::unique_ptr<Token> Lexer::peek_token() {
    auto token = get_token(false);
    return token;
}

char Lexer::get_char(void) {
    enum comment_type comment_type = NONE;
    // skip whitespace and comments
    while (index < length) {
        current_char = content[index++];
        if(current_char == '\n') {
            line++;
        }


        comment_type = is_comment();

        if (comment_type == CPP_COMMENT || comment_type == C_COMMENT) {
            skip_comment(comment_type);
            comment_type = NONE;
        }

        return current_char;
    }
    return EOF;
}

char Lexer::peek_char(void) {
    if (index < length) {
        return content[index];
    }

    return EOF;
}

// Skips characters until it gets to the end of a comment
void Lexer::skip_comment(Lexer::comment_type type) {
    while (peek_char() != EOF) {
        if (current_char == '\n' && type == CPP_COMMENT) {
            return;
        }

        if (is_comment() == C_COMMENT_END && type == C_COMMENT) {
            index += 2;
            return;
        }
        get_char();
    }
}

/**
 * Goes forward in the content untill it encounters non-whitespace.
 */
void Lexer::skip_wspace(void) {
    while (std::isspace(peek_char())) {
        get_char();
    }
}

enum Lexer::comment_type Lexer::is_comment(void) {
    char next_char = peek_char();
    if(current_char == '/' && next_char == '*') {
        return C_COMMENT;
    }

    if (current_char == '/' && next_char == '/') {
        return CPP_COMMENT;
    }

    if (current_char == '*' && next_char == '/') {
        return C_COMMENT_END;
    }

    return NONE;
}

char Lexer::interpret_escape(char c) {
    std::unordered_map<char, char> map = {{'a', 0x07}, {'b', 0x08},
        {'e', 0x1b}, {'f', 0x0c}, {'n', 0x0a}, {'r', 0x0d},
        {'t', 0x09}, {'v', 0x0b}, {'\\', 0x5c}, {'\'', 0x27},
        {'\"', 0x22}, {'?', 0x3f}};
    if (map.find(c) != map.end()) {
        return map.at(c);
    }
    return -1;
}


std::string Lexer::get_string() {
    std::string str;
    int start = line;

    bool escape = false;
    while(true) {
        char c = get_char();
        escape = c == '\\';

        if (escape) {
            escape = false;
            c = get_char();
            char escaped = interpret_escape(c);
            if (escaped != -1) {
                str.push_back(escaped);
            } else {

            }
            continue;
        }


        if (c == '"') {
            break;
        }

        if (c == EOF) {
            std::cout << "Error: missing \" on line " << start << "\n";
        }
        str.push_back(c);
    }
    return str;
}

enum token_type Lexer::get_token_type(std::string current) {
    enum token_type type;

    if (current == ":") {
        type = peek_char() == '=' ? ASSIGN : TYPE_DELIM;
    } else if (current == ".") {
        type = peek_char() == '.' ? RANGE : UNKNOWN;
    } else {
        if (!is_reserved(current)) {
            return IDENTIFIER;
        } else {
            type = reserved.at(current);
        }
    }

    return type;
}

extern std::string type_to_str(token_type type) {
    static std::unordered_map<token_type, std::string> to_str = {
                {NOT, "!"},  {ADDITION, "+"}, {SUBTRACTION, "-"},
                {AND, "&"}, {MULTIPLICATION, "*"}, {DIVISION, "/"},
                {LT, "<"}, {EQ, "="},
                {ASSIGN, ":="}, {STRING, "string"}, {LPARENTHESES, "("},
                {RPARENTHESES, ")"},
                {SEMICOLON, ";"}, {RANGE, ".."},

                {VAR, "var"},
                {FOR, "for"},
                {END, "end"},
                {IN, "in"},
                {DO, "do"},
                {READ, "read"},
                {PRINT, "print"},
                {INT, "int"},
                {STRING_TYPE, "string"},
                {BOOL, "bool"},
                {ASSERT, "assert"},
                {IF, "if"},
                {ELSE, "else"},
                {IDENTIFIER, "identifier"},
                {DIGIT, "digit"},
                {NO_SYMBOLS, "EOF"}
            };

            if (to_str.find(type) != to_str.end()) {
                return to_str.at(type);
            }
            return "could not find type";
}
