#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <string_view>

using lextype = std::vector<std::vector<std::string>*>;

lextype *lex(std::string_view filename);

#endif /* LEXER_H */
