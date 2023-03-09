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

    /**
     * Adds a symbol to the symbol table.
     *
     * @return true succeeding to add to table, false if not.
     */
    bool add_symbol(std::string_view symbol, std::unique_ptr<Literal> value);

    /**
     * Adds a symbol to the symbol table.
     *
     * @return true succeeding to add to table, false if not.
     */
    bool add_symbol(std::string_view symbol, int type);
    Literal *get_symbol(std::string_view symbol);

    /**
     * Checks whether a symbol is present in the symbol table.
     */
    bool exists(std::string_view symbol);
};

extern SymbolTable symbol_table;

#endif /* SYMBOLS_H */
