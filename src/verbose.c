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
            c(TOKEN_EOF);
            c(TOKEN_INT);
            c(TOKEN_NAME);
            c(TOKEN_COLON);
            c(TOKEN_INDENT);
            c(TOKEN_DEDENT);
            c(TOKEN_NEWLINE);
            c(TOKEN_RETURN);
            c(TOKEN_DECLARE);
            c(TOKEN_OPENPAREN);
            c(TOKEN_CLOSEPAREN);
            c(TOKEN_RETURNTYPE);
            #undef c
        }

        if (tokens->type == TOKEN_INT)
            printf(" value = %d", tokens->data.value);
        if (tokens->type == TOKEN_NAME)
            printf(" name = %s", tokens->data.name);
        if (tokens->type == TOKEN_NEWLINE)
            printf(" indent level = %d", tokens->data.indentlevel);
        printf("\n");
    } while (tokens++->type != TOKEN_EOF);

    printf("\n");
}


void print_asts(const AstStatement *statements)
{
    printf("AST of the \"%s\":\n", statements->location.filename);

    do {
        printf("    Line %d: ", statements->location.line);
        switch(statements->type) {
            #define c(x) case x: printf(#x); break
            c(AST_STMT_DECLARE);
            c(AST_STMT_CALL);
            c(AST_STMT_EOF);
            #undef c
        }

        switch(statements->type) {
        case AST_STMT_DECLARE:
            printf(" funcname=\"%s\"", statements->data.declare.funcname);
            break;
        case AST_STMT_CALL:
            printf(" funcname=\"%s\" arg=%d",
                statements->data.call.funcname,
                statements->data.call.arg);
            break;
        case AST_STMT_EOF:
            break;
        }

        printf("\n");

    } while (statements++->type != AST_STMT_EOF);

    printf("\n");
}

void free_ast(struct AstStatement *statements)
{
    free(statements);
}