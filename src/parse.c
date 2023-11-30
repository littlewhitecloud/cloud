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

static AstFunctionSignature parse_function_signature(const Token **tokens) {
    // declare funcname(argtype argname, ...) -> rettype
    AstFunctionSignature res = {.location=(*tokens)->location};

    // declare keyword already ate

    if ((*tokens)->type != TOKEN_NAME)
        raise_parse_error(*tokens, "declare function name");
    sstrcpy(res.funcname, (*tokens)->data.name);
    ++*tokens;

    eat_openparen(tokens);

    while ((*tokens)->type != TOKEN_CLOSEPAREN) {
        res.args++;

        parse_type(tokens); // type of the argument

        if ((*tokens)->type != TOKEN_NAME) // name of the argument
            raise_parse_error(*tokens, "an argument name");
        ++*tokens;
        // break;
    }
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
        case TOKEN_NAME:
            res.type = AST_STMT_CALL;
            res.data.call = parse_call(tokens);
            break;

        case TOKEN_RETURN:
            ++*tokens;
            res.type = AST_STMT_RETURN;
            res.data.retvalue = parse_expression(tokens);
            break;

        default:
            raise_parse_error(*tokens, "a statement");
    }

    eat_newline(tokens);
    return res;
}

static AstBody parse_body(const Token **tokens) {
    /*
    ...:
        statement
    */
    if ((*tokens)->type != TOKEN_COLON)
        raise_parse_error(*tokens, "':' followed by a new line with more indentation");
    ++*tokens;

    if ((*tokens)->type != TOKEN_NEWLINE)
        raise_parse_error(*tokens, "a new line with more indentation after ':'");
    ++*tokens;

    if ((*tokens)->type != TOKEN_INDENT)
        raise_parse_error(*tokens, "more indentation in the newline");
    ++*tokens;

    List(AstStatement) res = {0};
    while ((*tokens)->type != TOKEN_DEDENT)
        Append(&res, parse_statement(tokens));
    ++*tokens;

    return (AstBody){.statements = res.ptr, .statments = res.len};
}

static AstToplevelNode parse_toplevel_node(const Token **tokens) {
    AstToplevelNode res = {.location = (*tokens)->location};

    switch ((*tokens)->type) {
        case TOKEN_DECLARE:
            ++*tokens; // eat
            res.type = AST_TOPN_DECLARE;
            res.data.declare_signature = parse_function_signature(tokens);
            eat_newline(tokens);
            break;

        case TOKEN_EOF:
            res.type = AST_TOPN_EOF;
            break;
        
        default:
            ++*tokens;
            res.type = AST_TOPN_DEFINE;
            res.data.function_define.signature = parse_function_signature(tokens);
            res.data.function_define.body = parse_body(tokens);
            break;
    }

    return res;
}

AstToplevelNode *parse(const Token *tokens) {
    List(AstToplevelNode) res = {0};
    while (tokens->type != TOKEN_EOF)
        Append(&res, parse_toplevel_node(&tokens));

    Append(&res, (AstToplevelNode){.location = tokens->location, .type = AST_TOPN_EOF});
    return res.ptr;
}
