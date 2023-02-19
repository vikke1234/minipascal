#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <string>
#include <variant>
#include <unordered_map>
#include <memory>

struct value {
    union {
        std::string str;
        std::int64_t num;
        bool b;
    };

    enum {STR, BOOL, INT} tag;
};

using value_type = std::variant<std::string, int, bool>;
class SymbolTable {
    std::unique_ptr<std::unordered_map<std::string, value_type>> symbols;


public:
    SymbolTable() : symbols{} {}
    void add_symbol(std::string_view symbol);
    value_type &get_symbol(std::string_view symbol);
};

#endif /* SYMBOLS_H */
