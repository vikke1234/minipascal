#include "analysis.h"
#include "expr.h"
#include <memory>

bool Analyser::analyse() {
    std::unique_ptr<StatementList> program = parser.parse_file();
    const StatementList * next = program.get();
    const Expr *current = next->get_statement();
    std::cout << "Semantical analysis\n";

    bool has_error = false;
    do {
        has_error = has_error || current->analyse();
        next = next->get_next();
    } while(next != nullptr);

    return has_error;
}
