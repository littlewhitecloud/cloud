#include "compiler.h"

void print_tokens(const Token *tokens) {
    printf("Tokens of the \"%s\":\n", tokens->location.filename);

    int lastline = -1;
    do {
        if (tokens->location.line != lastline) {
            printf("    Line %d:\n", tokens->location.line);
            lastline = tokens->location.line;
        }
        printf("\t");
        switch(tokens->type) {
            #define c(x) case x: printf(#x); break
            c(TOKEN_INT);
            c(TOKEN_OPENPAREN);
            c(TOKEN_CLOSEPAREN);
            c(TOKEN_NAME);
            c(TOKEN_NEWLINE);
            c(TOKEN_EOF);
            c(TOKEN_DECLARE);
            #undef f
        }

        if (tokens->type == TOKEN_INT) {
            printf(" value = %d", tokens->data.value);
        }
        if (tokens->type == TOKEN_NAME) {
            printf(" name = %s", tokens->data.string);
        }
        printf("\n");
    } while (tokens++->type != TOKEN_EOF);

    printf("\n");
}
