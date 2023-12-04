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

static int parse_expression(const Token **tokens)
{
    if ((*tokens)->type != TOKEN_INT)
        raise_parse_error(*tokens, "an integer");
    return (*tokens)++->data.value;
}

static AstFunctionSignature parse_signature(const Token **tokens)
{
    // declare/def/fn funcname(argtype argname, ...) -> rettype
    AstFunctionSignature res = {.location = (*tokens)->location};

    ++*tokens; // eat keyword

    if ((*tokens)->type != TOKEN_NAME)
        raise_parse_error(*tokens, "function name");
    sstrcpy(res.funcname, (*tokens)->data.name);
    ++*tokens;

    eat_openparen(tokens);
    while ((*tokens)->type != TOKEN_CLOSEPAREN) {
        // arg: type
        res.nargs++;

        if ((*tokens)->type != TOKEN_NAME)
            raise_parse_error(*tokens, "an argument name");
        ++*tokens;

        if ((*tokens)->type != TOKEN_COLON)
            raise_parse_error(*tokens, "':' and a type after the argument");
        ++*tokens;

        parse_type(tokens);

        break;
    }

    eat_closeparen(tokens);

    if ((*tokens)->type != TOKEN_ARROW)
        raise_parse_error(*tokens, "a '->' after ')'");
    ++*tokens;
    parse_type(tokens); // rettype

    return res;
}

static AstCall parse_call(const Token **tokens)
{
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

static AstStatement parse_statement(const Token **tokens)
{
    AstStatement res = {.location = (*tokens)->location};

    switch ((*tokens)->type) {
        case TOKEN_NAME:
            res.type = AST_STMT_CALL;
            res.data.call = parse_call(tokens);
            break;
        
        case TOKEN_RETURN:
            ++*tokens;
            res.type = AST_STMT_RETURN;
            res.data.returnvalue = parse_expression(tokens);
            break;

        default:
            raise_parse_error(*tokens, "a statement");
    }

    eat_newline(tokens);
    return res;
}

static AstBody parse_body(const Token **tokens)
{
    if ((*tokens)->type != TOKEN_COLON)
        raise_parse_error(*tokens, "a colon followed by a newline with more indentation");
    ++*tokens;

    if ((*tokens)->type != TOKEN_NEWLINE)
        raise_parse_error(*tokens, "a newline with more indentation after a colon");
    ++*tokens;

    if ((*tokens)->type != TOKEN_INDENT)
        raise_parse_error(*tokens, "more indentation after a colon");
    ++*tokens;

    List(AstStatement) res = {0};
    while ((*tokens)->type != TOKEN_DEDENT)
        Append(&res, parse_statement(tokens));
    ++*tokens;
    return (AstBody){.statements = res.ptr, .nstatements = res.len};
}

static AstToplevelNode parse_toplevel_node(const Token **tokens)
{
    AstToplevelNode res = {.location = (*tokens)->location};
    switch ((*tokens)->type)
    {
        case TOKEN_DECLARE:
            res.type = AST_TOPN_DECLARE;
            res.data.declaresignature = parse_signature(tokens);
            eat_newline(tokens);
            break;
        
        case TOKEN_EOF:
            res.type = AST_TOPN_EOF;
            break;
        
        case TOKEN_DEFINE:
            res.type = AST_TOPN_DEFINE;
            res.data.functiondefine.signature = parse_signature(tokens);
            res.data.functiondefine.body = parse_body(tokens);
            break;
        
        default:
            raise_parse_error(*tokens, "keyword `declare` or `def / fn`");
    }
    return res;
}

AstToplevelNode *parse(const Token *tokens)
{
    List(AstToplevelNode) res = {0};
    do {
        Append(&res, parse_toplevel_node(&tokens));
    } while (res.ptr[res.len - 1].type != AST_TOPN_EOF);

    return res.ptr;
}