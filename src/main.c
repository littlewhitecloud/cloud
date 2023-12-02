#include <stdbool.h>

#include "compiler.h"

int main(int argc, char **argv)
{
    const char *filename;
    bool verbose;
    if (argc == 3 && !strcmp(argv[2], "-v"))
    {
        verbose = true;
        filename = argv[2];
    }
    else if (argc == 2)
    {
        verbose = false;
        filename = argv[1];
    }
    else
    {
        fprintf(stderr, "Usage: %s [-v] filename\n", argv[0]);
        return 2;
    }

    Token *tokens = tokenize(argv[1]);
    if (verbose)
        print_tokens(tokens);

    AstToplevelNode *ast = parse(tokens);
    free(tokens);

    if (verbose)
        print_asts(ast);

    LLVMModuleRef module = codegen(ast);
    free(ast);

    char *s = LLVMPrintModuleToString(module);
    if (verbose)
        printf("LLVM IR: \n\n%s", s);

    FILE *f = fopen("tmp.bc", "wb");
    fprintf(f, "%s", s);
    fclose(f);

    LLVMDisposeMessage(s);
    LLVMDisposeModule(module);

    #ifdef _WIN32
        return system("clang -Wno-override-module -o tmp.exe tmp.bc && tmp.exe");
    #else
        return system("/usr/lib/llvm-11/bin/clang -Wno-override-module -o tmp tmp.bc && .//tmp");
    #endif
}
