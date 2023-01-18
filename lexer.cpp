#include <cctype>
#include <fstream>
#include <string_view>
#include <iostream>
#include <sstream>

#include "lexer.h"

std::string read_file(std::string_view filename);
using identifier = int (*)(int);

/**
 * Parses the current word or number, identifies which by the identifier
 * function `f`
 *
 * @param content - file content
 * @param f       - identifier function (isalpha, isdigit)
 * @param i       - index in content
 * @param length  - length of content
 */
std::string parse(std::string_view content, identifier f,
                       std::size_t &i, const std::size_t length) {

    std::string word = std::string();

    while(f(content[i]) && i < length) {
        word.push_back(content[i]);
        i++;
    }

    return word;
}


std::string read_file(std::string_view filename) {
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

lextype *lex(std::string_view filename) {
    lextype *lexed = new lextype;
    std::vector<std::string> *statement = new std::vector<std::string>;

    const std::string content = read_file(filename);
    const std::size_t length = content.length();
    std::string current = std::string();

    std::cout << "lexing\n";
    for (std::size_t i = 0; i < length;) {
        if(std::isspace(content[i])) {
            i++;
            continue;
        }

        if(std::isalpha(content[i])) {
            current = parse(content, isalnum, i, length);
        } else if(std::isdigit(content[i])) {
            current = parse(content, isdigit, i, length);
        } else {
            current = std::string(1, content[i++]);
        }

        std::cout << current << "\n";
        statement->push_back(current);
    }
    return lexed;
}

