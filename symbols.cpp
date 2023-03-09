#include "symbols.h"
#include "expr.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>


SymbolTable symbol_table;


bool SymbolTable::add_symbol(std::string_view symbol,
        std::unique_ptr<Literal> value) {
    symbols[symbol.data()] = std::move(value);
}

Literal *SymbolTable::get_symbol(std::string_view str) {
    return symbols.at(str.data()).get();
}
