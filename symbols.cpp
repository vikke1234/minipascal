#include "symbols.h"
#include <string>
#include <unordered_map>
#include <variant>

void SymbolTable::add_symbol(std::string_view symbol) {
    (*symbols.get())[symbol.data()] = 0;
}

value_type &SymbolTable::get_symbol(std::string_view str) {
    return symbols.get()->at(str.data());
}
