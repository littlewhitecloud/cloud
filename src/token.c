#include <errno.h>

#include "utils.h"
#include "compiler.h"

static inline bool is_identifier_start(char c) { return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_'; }

static inline bool is_identifier_body(char c) { return is_identifier_start(c) || ('0' <= c && c <= '9'); }

static char read_byte(State *st)
{
    if (st->location.line == 0) {
        st->location.line++;
        return '\n';
    }

    int c = fgetc(st->file);
    if (c == EOF)
    { // end of the file
        if (ferror(st->file))
            raise_error(st->location, "cannot open file: %s", strerror(errno));
        return '\0'; // return a zero byte
    }
    if (c == '\r')
    {
        c = fgetc(st->file);
        if (c == EOF && ferror(st->file))
            raise_error(st->location, "cannot open file: %s", strerror(errno));
        if (c != '\n')
            raise_error(st->location, "file contains a `CR` byte");
    }
    if (c == '\n')
        st->location.line++;

    return c;
}

static void unread_byte(State *st, char c)
{
    if (c == '\0')
        return;
    ungetc(c, st->file);
    if (c == '\n')
        st->location.line--;
}

static char read_char_in_string(State *st)
{
    // already ate '
    char c = read_byte(st);
    if (c == '\'')
        raise_warning(st->location, "empty character literral: ''");
    if (c == '\\')
    {
        char n = read_byte(st); // next char
        switch (n)
        { // TODO: add more
        case 'n': c = '\n'; break;
        case '\\': c = '\\'; break;
        case '\'': c = '\''; break;
        case '0': c = 0; break;
        default:
            raise_error(st->location, "unknown escape character: '\\%c'", n);
        }
    }

    read_byte(st); // eat '
    return c;
}

static void read_identifier(State *st, char byte, char (*dest)[100])
{
    int len = 0;

    if (!is_identifier_start(byte))
        raise_error(st->location, "identifier cannot start with '%c'", byte);

    memset(*dest, 0, sizeof *dest);
    (*dest)[len++] = byte;

    while (1)
    {
        if (len == sizeof *dest - 1) // out of range
            raise_error(st->location, "name \"%s\" is too long", *dest);

        char c = read_byte(st);
        if (!is_identifier_body(c))
        {
            unread_byte(st, c);
            return;
        }
        (*dest)[len++] = c;
    }
}

static void read_indent(State *st, Token *t)
{
    t->type = TOKEN_NEWLINE;
    while (1)
    {
        char c = read_byte(st);
        if (c == ' ')
            t->data.indentlevel++;
        else if (c == '\n')
            t->data.indentlevel = 0;
        else if (c == '\0') {
            t->type = TOKEN_EOF; // TODO: test it
            return;
        } else {
            unread_byte(st, c);
            break;
        } 
    }
}

static Token read_token(State *st)
{
    Token t = {.location = st->location};

    while (1) {
        char c = read_byte(st);
        char n;
        switch (c)
        {
        case ' ':
            continue;
        case '\n':
            read_indent(st, &t);
            break;
        case '\0':
            t.type = TOKEN_EOF;
            break;
        case '(':
            t.type = TOKEN_OPENPAREN;
            break;
        case ')':
            t.type = TOKEN_CLOSEPAREN;
            break;
        case '-':
            n = read_byte(st);
            if (!(n == '>')) raise_error(st->location, "unexpected byte '%c' after '-'", n);
            t.type = TOKEN_RETURNTYPE;
            break;
        case ':':
            t.type = TOKEN_COLON;
            break;
        case '"':
        case '\'':
            t.type = TOKEN_INT;
            t.data.value = read_char_in_string(st);
            break;
        default:
            if (!is_identifier_start(c))
                raise_error(st->location, "unexpected byte '%c' (%#02x)", c, (int)c);
            t.type = TOKEN_NAME;
            read_identifier(st, c, &t.data.name);
            if (!strcmp(t.data.name, "declare"))
                t.type = TOKEN_DECLARE;
            break;
        }
        return t;
    }
}

Token *tokenize_without_indent(const char *filename)
{
    State st = {.location.filename = filename, .file = fopen(filename, "rb")};
    if (!st.file)
        raise_error(st.location, "cannot open file: %s", strerror(errno));

    List(Token) tokens = {0};
    while (tokens.len == 0 || tokens.ptr[tokens.len - 1].type != TOKEN_EOF)
        Append(&tokens, read_token(&st));
    fclose(st.file);

    return tokens.ptr;
}

Token *tokenize(const char *filename)
{
    List(Token) tokens = {0};
    Token *t = tokenize_without_indent(filename);
    int level = 0;
    do 
    {
        if (t->type == TOKEN_EOF)
        {
            while (level) {
                Append(&tokens, (Token){.location = t->location, .type = TOKEN_DEDENT});
                level -= 4;
            }
        }
        Append(&tokens, *t);

        if (t->type == TOKEN_NEWLINE) {
            if (t->data.indentlevel % 4 != 0)
                raise_error(t->location, "indentation must be a multiple of 4 spaces");

            Location after = t->location;
            after.line++;
            while (level < t->data.indentlevel) {
                Append(&tokens, (Token){.location = after, .type = TOKEN_INDENT});
                level += 4;
            }
            while (level > t->data.indentlevel) {
                Append(&tokens, (Token){.location=after, .type=TOKEN_DEDENT});
                level -= 4;
            }
        }
    } while (t++->type != TOKEN_EOF);

    free(t);
    return tokens.ptr;
}
