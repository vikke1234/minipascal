#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"
class Interpreter {
    Parser parser;

public:
    Interpreter(std::string_view filename) : parser{Parser(filename)} {}
    void run();
};
#endif // INTERPRETER_H
