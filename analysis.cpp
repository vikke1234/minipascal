#include "analysis.h"
#include "expr.h"
#include <memory>

bool Analyser::analyse(StatementList *ast) {
    const StatementList * next = ast;
    const Expr *current = nullptr;

    bool has_error = false;
    while(next != nullptr) {
        current = next->get_statement();
        if (current == nullptr) {
            break;
        }
        has_error = has_error || current->analyse();
        next = next->get_next();
    };

    return has_error;
}
