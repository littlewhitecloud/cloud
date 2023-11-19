#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdnoreturn.h>

typedef struct Location Location;
typedef struct State State;
typedef struct Token Token;

enum TokenType
{
    // types
    TOKEN_INT,

    // operators
    TOKEN_CLOSEPAREN,
    TOKEN_OPENPAREN,

    // syntaxs
    TOKEN_NEWLINE,
    TOKEN_NAME,
    TOKEN_EOF,

    // keywords
    TOKEN_DECLARE,
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

struct Token
{
    enum TokenType type; // type of the token
    Location location;   // location of the token
    union
    {
        int value;    // TOKEN_INT
        char *string; // TOKEN_NAME
    } data;
};

// Token
Token *tokenize(const char *filename);

// Verbose
void print_tokens(const Token *tokens);

// Error
noreturn void raise_error(Location location, const char *fmt, ...);
noreturn void raise_warning(Location location, const char *fmt, ...);

#endif
