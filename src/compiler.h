#ifndef COMPILER_H
#define COMPILER_H

#include "llvm-c/Core.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdnoreturn.h>


typedef struct Location Location;
typedef struct State State;
typedef struct Token Token;
typedef struct AstCall AstCall;
typedef struct AstDeclare AstDeclare;
typedef struct AstStatement AstStatement;

enum TokenType
{
    // types
    TOKEN_INT,

    // operators
    TOKEN_RETURNTYPE,
    TOKEN_CLOSEPAREN,
    TOKEN_OPENPAREN,

    // syntaxs
    TOKEN_NEWLINE,
    TOKEN_NAME,
    TOKEN_EOF,

    // keywords
    TOKEN_DECLARE,
};

enum AstStatementType
{
    AST_STMT_DECLARE,
    AST_STMT_CALL,
    AST_STMT_EOF,
};

struct AstCall {
    char funcname[100];
    int arg;
};

struct AstDeclare {
    char funcname[100];
};

struct Location
{
    int line;
    const char *filename;
};

struct State
{
    Location location;
    FILE *file;
};

struct AstStatement
{
    enum AstStatementType type;
    Location location;
    union {
        AstDeclare declare;
        AstCall call;
    } data;
};


struct Token
{
    enum TokenType type; // type of the token
    Location location;   // location of the token
    union
    {
        int value;    // TOKEN_INT
        char name[100]; // TOKEN_NAME
    } data;
};

// Token
Token *tokenize(const char *filename);

// Verbose
void print_tokens(const Token *tokens);
void print_asts(const AstStatement *statements);

// Error
noreturn void raise_error(Location location, const char *fmt, ...);
noreturn void raise_warning(Location location, const char *fmt, ...);
noreturn void raise_parse_error(const Token *token, const char *expect);

// Codegen
LLVMModuleRef codegen(const AstStatement *ast);

#endif
