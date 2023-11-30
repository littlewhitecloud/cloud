#include "llvm-c/Types.h"
#include "compiler.h"

static LLVMValueRef generate_declare(LLVMModuleRef module, const AstFunctionSignature *sig)
{
    if (LLVMGetNamedFunction(module, sig->funcname))
        raise_warning(sig->location, "a function named \"%s\" already declared");

    LLVMTypeRef i32type = LLVMInt32TypeInContext(LLVMGetGlobalContext());
    LLVMTypeRef *argtype = malloc(sig->args * sizeof(argtype[0]));

    for (int cnt = 0; cnt < sig->args; cnt++)
        argtype[cnt] = i32type;

    LLVMTypeRef functype = LLVMFunctionType(i32type, argtype, sig->args, false);

    free(argtype);

    return LLVMAddFunction(module, sig->funcname, functype);
}

static void generate_statement(LLVMBuilderRef builder, LLVMModuleRef module, const AstStatement *stmt)
{
    LLVMTypeRef i32type = LLVMInt32TypeInContext(LLVMGetGlobalContext());

    switch (stmt->type)
    {
        case AST_STMT_CALL:
            LLVMValueRef func = LLVMGetNamedFunction(module, stmt->data.call.funcname);
            if (!func)
                raise_error(stmt->location, "function named \"%s\" not found", stmt->data.call.funcname);

            LLVMTypeRef functype = LLVMTypeOf(func);
            LLVMValueRef arg = LLVMConstInt(i32type, stmt->data.call.arg, false);

            LLVMBuildCall2(builder, functype, func, &arg, 1, "putchar");

            break;
        case AST_STMT_RETURN:
            LLVMBuildRet(builder, LLVMConstInt(i32type, stmt->data.call.arg, false));
            break;
    }
}

static void generate_define(LLVMBuilderRef builder, LLVMModuleRef module, const AstFunctionDefine *def)
{
    LLVMValueRef function = generate_declare(module, &def->signature);

    LLVMBasicBlockRef block = LLVMAppendBasicBlockInContext(LLVMGetGlobalContext(), function, "block");
    LLVMPositionBuilderAtEnd(builder, block);
    for (int cnt = 0; cnt < def->body.statements; cnt++)
        generate_statement(builder, module, &def->body.statements[cnt]);
}

LLVMModuleRef codegen(const AstToplevelNode *ast)
{
    LLVMModuleRef module = LLVMModuleCreateWithName("module");
    LLVMSetSourceFileName(module, ast->location.filename, strlen(ast->location.filename));

    LLVMBuilderRef builder = LLVMCreateBuilder();

    for ( ; ; ast++)
    {
        switch (ast->type)
        {
            case AST_TOPN_DECLARE:
                generate_declare(module, &ast->data.declare_signature);
                break;
            case AST_TOPN_DEFINE:
                generate_define(builder, module, &ast->data.function_define);
                break;

            case AST_TOPN_EOF:
                LLVMDisposeBuilder(builder);
                break;
        }
    }
}
