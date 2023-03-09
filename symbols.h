#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <string>
#include <unordered_map>
#include <variant>
#include <memory>

class Literal;

class SymbolTable {
    std::unordered_map<std::string, std::unique_ptr<Literal>> symbols;

public:
    SymbolTable() : symbols{} {}
    bool add_symbol(std::string_view symbol, std::unique_ptr<Literal> value);
    Literal *get_symbol(std::string_view symbol);
};

extern SymbolTable symbol_table;

#endif /* SYMBOLS_H */
