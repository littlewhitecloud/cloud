#ifndef COMPILER_H
#define COMPILER_H

#include "llvm-c/Core.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdnoreturn.h>



typedef struct State State;
typedef struct Token Token;
typedef struct AstCall AstCall;
typedef struct AstBody AstBody;
typedef struct Location Location;
typedef struct AstStatement AstStatement;
typedef struct AstToplevelNode AstToplevelNode;
typedef struct AstFunctionDefine AstFunctionDefine;
typedef struct AstFunctionSignature AstFunctionSignature;


enum TokenType
{
    // types
    TOKEN_INT,

    // operators
    TOKEN_RETURNTYPE,
    TOKEN_CLOSEPAREN,
    TOKEN_OPENPAREN,
    TOKEN_COLON,

    // syntaxs
    TOKEN_NEWLINE,
    TOKEN_INDENT,
    TOKEN_DEDENT,
    TOKEN_NAME,
    TOKEN_EOF,

    // keywords
    TOKEN_DECLARE,
    TOKEN_RETURN,
};

enum AstStatementType
{
    AST_STMT_CALL,
    AST_STMT_RETURN,
};

enum AstToplevelNodeType
{
    AST_TOPN_EOF,
    AST_TOPN_DECLARE,
    AST_TOPN_DEFINE,
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

struct AstCall
{
    char funcname[100];
    int arg;
};

struct AstBody
{
    AstStatement *statements;
    int statments;
};

struct AstFunctionSignature
{
    Location location;
    char funcname[100];
    int args;
};

struct AstFunctionDefine
{
    AstFunctionSignature signature;
    AstBody body;
};

struct AstStatement
{
    enum AstStatementType type; // type of the statement
    Location location; // location of the statement
    union {
        int retvalue; // AST_STMT_RETURN
        AstCall call; // AST_STMT_CALL
    } data;
};

struct AstToplevelNode
{
    enum AstToplevelNodeType type;
    Location location;
    union {
        AstFunctionSignature declare_signature;
        AstFunctionDefine function_define;
    } data;
};

struct Token
{
    enum TokenType type; // type of the token
    Location location;   // location of the token
    union
    {
        int value;    // TOKEN_INT
        int indentlevel;
        char name[100]; // TOKEN_NAME
    } data;
};

// Token
Token *tokenize(const char *filename);

// Parse
AstToplevelNode *parse(const Token *tokens);

// Codegen
LLVMModuleRef codegen(const AstToplevelNode *ast);

// Verbose
void print_tokens(const Token *tokens);
void print_asts(const struct AstToplevelNode *topnodelist);

// Error
noreturn void raise_error(Location location, const char *fmt, ...);
noreturn void raise_warning(Location location, const char *fmt, ...);
noreturn void raise_parse_error(const Token *token, const char *expect);


#endif
