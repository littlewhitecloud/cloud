#include <stdio.h>
#include <stdarg.h>

#include "compiler.h"

noreturn void raise_error(Location location, const char *fmt, ...) {
    fflush(stdout);

    fprintf(stderr, "CompilerError:\n");
    fprintf(stderr, "    File \"%s\", line %d\n", location.filename, location.line);

    va_list msg;
    va_start(msg, fmt);
    vfprintf(stderr, fmt, msg);
    va_end(msg);

    fprintf(stderr, "\n");
    exit(1);
}

void raise_warning(Location location, const char *fmt, ...) {
    fflush(stdout);

    fprintf(stderr, "Warning:\n");
    fprintf(stderr, "    File \"%s\", line %d\n", location.filename, location.line);

    va_list msg;
    va_start(msg, fmt);
    vfprintf(stderr, fmt, msg);
    va_end(msg);

    fprintf(stderr, "\n");
}

noreturn void raise_parse_error(const Token *token, const char *expect) {
    char res[100];
    switch (token->type)
    {
    case TOKEN_INT:
        strcpy(res, "an integer");
        break;
    case TOKEN_OPENPAREN:
        strcpy(res, "'('");
        break;
    case TOKEN_CLOSEPAREN:
        strcpy(res, "'('");
    case TOKEN_NAME:
        snprintf(res, sizeof res, "a variable named %s", token->data.name);
        break;
    case TOKEN_EOF:
        strcpy(res, "end of the file");
        break;
    case TOKEN_DECLARE:
        strcpy(res, "keyword `declare`");
        break;
    default:
        break;
    }
    raise_error(token->location, "expected %s got %s", expect, res);
}