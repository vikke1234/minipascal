#include "symbols.h"
#include "expr.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>


SymbolTable symbol_table;


bool SymbolTable::add_symbol(std::string_view symbol,
        std::unique_ptr<Literal> value) {

    if(exists(symbol)) {
        return false;
    }

    symbols[symbol.data()] = std::move(value);
    return true;
}

bool SymbolTable::add_symbol(std::string_view symbol, int type) {
    if(exists(symbol)) {
        return false;
    }

    switch(type){
        case 0:
            symbols[symbol.data()] = std::make_unique<Literal>(0);
            break;
        case 1:
            symbols[symbol.data()] = std::make_unique<Literal>("");
            break;
        case 2:
            symbols[symbol.data()] = std::make_unique<Literal>(false);
            break;
        default:
            std::cout << "Invalid type\n";
            exit(1);
            break;
    }
    return true;
}

bool SymbolTable::exists(std::string_view symbol) {
    return symbols.find(symbol.data()) != symbols.end();
}

Literal *SymbolTable::get_symbol(std::string_view str) {
    return symbols.at(str.data()).get();
}

bool SymbolTable::set_value(std::string_view symbol, Literal *literal) {
    if (symbols.find(symbol.data()) != symbols.end()) {
        *symbols[symbol.data()] = *literal;
    }
    return false;
}
