#include "utils.h"
#include "compiler.h"

static void eat_newline(const Token **tokens)
{
    if ((*tokens)->type != TOKEN_NEWLINE)
        raise_parse_error(*tokens, "a newline");
    ++*tokens;
}

static void eat_openparen(const Token **tokens)
{
    if ((*tokens)->type != TOKEN_OPENPAREN)
        raise_parse_error(*tokens, "a '(' before arguments");
    ++*tokens;
}

static void eat_closeparen(const Token **tokens)
{
    if ((*tokens)->type != TOKEN_CLOSEPAREN)
        raise_parse_error(*tokens, "a ')' after arguments");
    ++*tokens;
}

static void parse_type(const Token **tokens)
{
    if ((*tokens)->type != TOKEN_NAME || strcmp((*tokens)->data.name, "int")) {
        raise_parse_error(*tokens, "`int` type");
    }
    ++*tokens;
}

static int parse_expression(const Token **tokens) {
    if ((*tokens)->type != TOKEN_INT)
        raise_parse_error(*tokens, "an integer");
    return (*tokens)++->data.value;
}

static AstDeclare parse_declare(const Token **tokens) {
    // declare funcname(argtype argname, ...) -> rettype
    AstDeclare res;

    ++*tokens; // declare keyword should be checked

    if ((*tokens)->type != TOKEN_NAME)
        raise_parse_error(*tokens, "declare function name");
    sstrcpy(res.funcname, (*tokens)->data.name);
    ++*tokens;

    eat_openparen(tokens);

    parse_type(tokens);

    if ((*tokens)->type != TOKEN_NAME)
        raise_parse_error(*tokens, "an argument name");
    ++*tokens;

    eat_closeparen(tokens);

    if ((*tokens)->type != TOKEN_RETURNTYPE)
        raise_parse_error(*tokens, "a '->' after ')'");
    ++*tokens;

    parse_type(tokens); // rettype

    return res;
}

static AstCall parse_call(const Token **tokens) {
    // funcname(args, ...)
    AstCall res;
    
    if ((*tokens)->type != TOKEN_NAME)
        raise_parse_error(*tokens, "a function name");
    sstrcpy(res.funcname, (*tokens)->data.name);
    ++*tokens;

    eat_openparen(tokens);

    res.arg = parse_expression(tokens);

    eat_closeparen(tokens);

    return res;
}

static AstStatement parse_statement(const Token **tokens) {
    AstStatement res = {.location = (*tokens)->location};

    switch ((*tokens)->type) {
        case TOKEN_DECLARE:
            res.type = AST_STMT_DECLARE;
            res.data.declare = parse_declare(tokens);
            break;
        
        case TOKEN_NAME:
            res.type = AST_STMT_CALL;
            res.data.call = parse_call(tokens);
            break;
        
        default:
            raise_parse_error(*tokens, "a statement");
    }

    eat_newline(tokens);

    return res;
}

AstStatement *parse(const Token *tokens) {
    ++tokens;

    List(AstStatement) res = {0};
    while (tokens->type != TOKEN_EOF)
        Append(&res, parse_statement(&tokens));

    Append(&res, (AstStatement){.location = tokens->location, .type = AST_STMT_EOF});
    return res.ptr;
}