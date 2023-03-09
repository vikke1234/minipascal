#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "parser.h"

class Analyser {
    Parser parser;
    public:
        Analyser(Parser parser) : parser{parser} {}
        bool analyse();
};

#endif  // ANALYSIS_H
