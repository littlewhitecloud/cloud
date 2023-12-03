#include "llvm-c/Core.h"
#include "llvm-c/Types.h"

#include "compiler.h"

static LLVMValueRef codegen_declare(LLVMModuleRef module, const AstFunctionSignature *sig)
{
    if (LLVMGetNamedFunction(module, sig->funcname))
        raise_error(sig->location, "a function named \"%s\" already exists", sig->funcname);

    LLVMTypeRef i32type = LLVMInt32TypeInContext(LLVMGetGlobalContext());

    LLVMTypeRef *argtypes = malloc(sig->nargs * sizeof(argtypes[0]));
    for (int i = 0; i < sig->nargs; i++)
        argtypes[i] = i32type;

    LLVMTypeRef functype = LLVMFunctionType(i32type, argtypes, sig->nargs, false);
    free(argtypes);

    return LLVMAddFunction(module, sig->funcname, functype);
}

static void codegen_statement(LLVMModuleRef module, LLVMBuilderRef builder, const AstStatement *stmt)
{
    LLVMTypeRef i32type = LLVMInt32TypeInContext(LLVMGetGlobalContext());
    switch (stmt->type)
    {
        case AST_STMT_CALL:
        {
            LLVMValueRef func = LLVMGetNamedFunction(module, stmt->data.call.funcname);
            if (!func)
                raise_error(stmt->location, "function \"%s\" not found", stmt->data.call.funcname);

            LLVMTypeRef functype = LLVMTypeOf(func);
            LLVMValueRef arg = LLVMConstInt(i32type, stmt->data.call.arg, false);

            LLVMBuildCall2(builder, functype, func, &arg, 1, "putcharcall");
            break;
        }

        case AST_STMT_RETURN:
            LLVMBuildRet(builder, LLVMConstInt(i32type, stmt->data.call.arg, false));
            break;
    }
}

static void codegen_function(LLVMModuleRef module, LLVMBuilderRef builder, const AstFunctionDefine *def)
{
    LLVMValueRef func = codegen_declare(module, &def->signature);

    LLVMBasicBlockRef block = LLVMAppendBasicBlockInContext(LLVMGetGlobalContext(), func, "functionblock");

    LLVMPositionBuilderAtEnd(builder, block);
    for (int i = 0; i < def->body.nstatements; i++)
        codegen_statement(module, builder, &def->body.statements[i]);
}

LLVMModuleRef codegen(const struct AstToplevelNode *ast)
{
    LLVMModuleRef codegen(const struct AstToplevelNode *ast);
    LLVMModuleRef module = LLVMModuleCreateWithName("module");
    LLVMSetSourceFileName(module, ast->location.filename, strlen(ast->location.filename));

    LLVMBuilderRef builder = LLVMCreateBuilder();

    for ( ; ; ast++)
    {
        switch (ast->type)
        {
        case AST_TOPN_DECLARE:
            codegen_declare(module, &ast->data.declaresignature);
            break;

        case AST_TOPN_DEFINE:
            codegen_function(module, builder, &ast->data.functiondefine);
            break;

        case AST_TOPN_EOF:
            LLVMDisposeBuilder(builder);
            return module;
        }
    }
}