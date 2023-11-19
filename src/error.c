#include <stdio.h>
#include <stdarg.h>

#include "compiler.h"

noreturn void raise_error(Location location, const char *fmt, ...) {
    fprintf(stderr, "CompilerError:\n");
    fprintf(stderr, "\tFile \"%s\", line %d\n", location.filename, location.line);

    va_list msg;
    va_start(msg, fmt);
    vfprintf(stderr, fmt, msg);
    va_end(msg);

    fprintf(stderr, "\n");
    exit(1);
}

void raise_warning(Location location, const char *fmt, ...) {
    fprintf(stderr, "Warning:\n");
    fprintf(stderr, "\tFile \"%s\", line %d\n", location.filename, location.line);

    va_list msg;
    va_start(msg, fmt);
    vfprintf(stderr, fmt, msg);
    va_end(msg);

    fprintf(stderr, "\n");
}