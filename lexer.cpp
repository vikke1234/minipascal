#include <cctype>
#include <fstream>
#include <string_view>
#include <iostream>
#include <sstream>

#include "lexer.h"

/**
 * Parses the current word or number, identifies which by the identifier
 * function `f`
 *
 * @param content - file content
 * @param f       - identifier function (isalpha, isdigit)
 * @param i       - index in content
 * @param length  - length of content
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

struct Token Lexer::get_token() {

    std::string current = std::string();
    enum token_type type;

    // todo fix symbol bug, skips symbol if parse was called
    char c = get_char();
    if (std::isspace(c)) {
        skip_wspace();
        c = get_char();
    }

    if (c == EOF) {
        return {0, "", NO_SYMBOLS};
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

        if (type == ASSIGN) {
            current.push_back(get_char());
        } else if(type == STRING) {
            current = get_string();
        }
    }

    struct Token t = {line, current, type};
    return t;
}

/**
 * Gets the next character from the file
 *
 * Note: skips comments
 */
char Lexer::get_char(void) {
    enum comment_type comment_type = NONE;
    // skip whitespace and comments
    while (index < length) {
        current_char = content[index++];
        if(current_char == '\n') {
            line++;
        }

        comment_type = is_comment();

        if (comment_type) {
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
            return;
        }
        get_char();
    }
}

void Lexer::skip_wspace(void) {
    while (std::isspace(peek_char())) {
        get_char();
    }
}

enum Lexer::comment_type Lexer::is_comment(void) {
    char next_char = peek_char();
    if(current_char == '/' && next_char == '*') {
        return CPP_COMMENT;
    }

    if (current_char == '/' && next_char == '/') {
        return C_COMMENT;
    }

    if (current_char == '*' && next_char == '/') {
        return C_COMMENT_END;
    }

    return NONE;
}

std::string Lexer::get_string() {
    std::string str;
    int start = line;

    bool escape = false;
    while(true) {
        char c = get_char();

        if (escape) {
            escape = false;
            str.push_back(c);
            continue;
        }

        escape = c == '\\';

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
        // only symbol which depends on the next symbol
        type = peek_char() == '=' ? ASSIGN : TYPE_DELIM;
    } else {
        if (reserved.find(current) == reserved.end()) {
            return IDENTIFIER;
        } else {
            type = reserved.at(current);
        }
    }

    return type;
}
