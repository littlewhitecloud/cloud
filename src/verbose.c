#include "compiler.h"

void print_function_signature(const AstFunctionSignature *sig, int indent)
{
    printf("%*sfunction signature (line %d): int %s(", indent, "", sig->location.line, sig->funcname);
    for (int i = 0; i < sig->nargs; i++)
    {
        if (i)
            printf(", ");
        printf("int");
    }
    printf(")\n");
}

void print_tokens(const Token *tokens)
{
    printf("Tokens of the \"%s\":\n", tokens->location.filename);

    int lastline = -1;
    do
    {
        if (tokens->location.line != lastline)
        {
            printf("    Line %d:\n", tokens->location.line);
            lastline = tokens->location.line;
        }
        printf("\t");
        switch (tokens->type)
        {
#define c(x)        \
    case x:         \
        printf(#x); \
        break
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
        else if (tokens->type == TOKEN_NAME)
            printf(" name = %s", tokens->data.name);
        else if (tokens->type == TOKEN_NEWLINE)
            printf(" indent level = %d", tokens->data.indentlevel);
        printf("\n");
    } while (tokens++->type != TOKEN_EOF);

    printf("\n");
}

static void print_statement(const AstStatement *stmt, int indent)
{
    printf("%*s(line %d) ", indent, "", stmt->location.line);
    switch (stmt->type)
    {
#define f(x)        \
    case x:         \
        printf(#x); \
        break
        f(AST_STMT_CALL);
        f(AST_STMT_RETURN);
#undef f
    }

    switch (stmt->type)
    {
    case AST_STMT_CALL:
        printf(" funcname=\"%s\" arg=%d\n", stmt->data.call.funcname, stmt->data.call.arg);
        break;
    case AST_STMT_RETURN:
        printf(" returnvalue=%d\n", stmt->data.returnvalue);
        break;
    }
}

static void print_body(const AstBody *body, int indent)
{
    printf("%*sbody:\n", indent, "");
    for (int i = 0; i < body->nstatements; i++)
        print_statement(&body->statements[i], indent + 2);
}

void print_asts(const AstToplevelNode *topnodelist)
{
    printf("AST of the \"%s\" ---\n", topnodelist->location.filename);

    do
    {
        printf("line %d: ", topnodelist->location.line);

        switch (topnodelist->type)
        {
#define c(x)        \
    case x:         \
        printf(#x); \
        break
            c(AST_TOPN_DECLARE);
            c(AST_TOPN_DEFINE);
            c(AST_TOPN_EOF);
#undef c
        }
        printf("\n");

        switch (topnodelist->type)
        {
        case AST_TOPN_DECLARE:
            print_function_signature(&topnodelist->data.declaresignature, 2);
            break;
        case AST_TOPN_DEFINE:
            print_function_signature(&topnodelist->data.functiondefine.signature, 2);
            print_body(&topnodelist->data.functiondefine.body, 2);
            break;
        case AST_TOPN_EOF:
            break;
        }
        printf("\n");

    } while (topnodelist++->type != AST_TOPN_EOF);

    printf("\n");
}

void print_llvmir(char *s)
{
    printf("LLVM IR: \n\n%s", s);
}
