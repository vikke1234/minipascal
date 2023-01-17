#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

const char * usage = "./minipascal filename\n";

int main(int argc, char **argv) {
    if(argc == 1) {
        std::cout << usage;
        return 1;
    }

    std::string filename;

    int opt = 0;

    // added for future
    while (true) {
        opt = getopt(argc, argv, "v");
        if (opt == -1) {
            break;
        }

        switch(opt) { }
    }

    if (optind >= argc) {
        std::cout << "Expected filename";
        std::exit(EXIT_FAILURE);
    }

    filename = argv[optind];
    return EXIT_SUCCESS;
}
