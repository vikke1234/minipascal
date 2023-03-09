#include "analysis.h"
#include "expr.h"
#include <memory>

bool Analyser::analyse() {
    std::unique_ptr<StatementList> program = parser.parse_file();
    const StatementList * next = program.get();
    const Expr *current = nullptr;
    std::cout << "Semantical analysis\n";

    bool has_error = false;
    while(next != nullptr) {
        current = next->get_statement();
        if (current == nullptr) {
            break;
        }
        current->analyse();
        next = next->get_next();
    };

    return has_error;
}
