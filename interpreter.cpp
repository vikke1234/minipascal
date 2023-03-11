#include "interpreter.h"
#include "analysis.h"
#include <memory>

void Interpreter::run() {
    std::unique_ptr<StatementList> program = parser.parse_file();

    Analyser analyser;
    analyser.analyse(program.get());

    StatementList * next = program.get();
    Expr *current = nullptr;
    std::cout << "Semantical analysis\n";

    while(next != nullptr) {
        current = next->get_statement();
        if (current == nullptr) {
            break;
        }
        current->interpet();
        next = next->get_next();
    };

}
