#include "interpreter.h"
#include "analysis.h"
#include <cstdlib>
#include <memory>

void Interpreter::run() {
    std::unique_ptr<StatementList> program = parser.parse_file();

    Analyser analyser;
    if(analyser.analyse(program.get())) {
        std::cout << "Error occured, exitting" << std::endl;
        std::exit(1);
    }

    StatementList * next = program.get();
    Expr *current = nullptr;

    while(next != nullptr) {
        current = next->get_statement();
        if (current == nullptr) {
            break;
        }
        current->interpet();
        next = next->get_next();
    };

}
