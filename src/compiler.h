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
typedef struct AstBody AstBody;
typedef struct AstDeclare AstDeclare;
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
    TOKEN_RETURN,
    TOKEN_DECLARE,
};

enum AstToplevelNodeType
{
    AST_TOPN_EOF,
    AST_TOPN_DECLARE,
    AST_TOPN_DEFINE,
};

enum AstStatementType
{
    AST_STMT_CALL,
    AST_STMT_RETURN,
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

struct AstFunctionSignature
{
    Location location;
    char funcname[100];
    int nargs;
};

struct AstBody
{
    AstStatement *statements;
    int nstatements;
};

struct AstFunctionDefine
{
    AstFunctionSignature signature;
    AstBody body;
};

struct AstToplevelNode
{
    Location location;
    enum AstToplevelNodeType type;
    union
    {
        AstFunctionSignature declaresignature;
        AstFunctionDefine functiondefine;
    } data;
};

struct AstStatement
{
    enum AstStatementType type;
    Location location;
    union {
        AstCall call;
        int returnvalue;
    } data;
};


struct Token
{
    enum TokenType type; // type of the token
    Location location;   // location of the token
    union
    {
        int value;       // TOKEN_INT
        int indentlevel; // TOKEN_INDENT
        char name[100];  // TOKEN_NAME
    } data;
};

// Token
Token *tokenize(const char *filename);

// Parse
AstToplevelNode *parse(const Token *tokens);

// Verbose
void print_tokens(const Token *tokens);
void print_asts(const AstToplevelNode *topnodelist);
void print_llvmir(LLVMModuleRef module, char *s);

// Error
noreturn void raise_error(Location location, const char *fmt, ...);
noreturn void raise_warning(Location location, const char *fmt, ...);
noreturn void raise_parse_error(const Token *token, const char *expect);

// Codegen
LLVMModuleRef codegen(const AstToplevelNode *ast);

#endif
