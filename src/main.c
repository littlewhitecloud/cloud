#include "compiler.h"

int main(int argc, char **argv)
{
    const char *filename;
    bool verbose;
    if (argc == 3 && !strcmp(argv[2], "-v")) {
        verbose = true;
        filename = argv[2];
    } else if (argc == 2) {
        verbose = false;
        filename = argv[1];
    } else {
        fprintf(stderr, "Usage: %s filename [-v]\n", argv[0]);
        return 2;
    }

    Token *tokens = tokenize(argv[1]);
    if (verbose) print_tokens(tokens);
    return 0;
}
