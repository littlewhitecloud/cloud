#include "llvm-c/Types.h"
#include "compiler.h"


LLVMModuleRef codegen(const AstToplevelNode *ast)
{
    LLVMModuleRef module = LLVMModuleCreateWithName("module");
    LLVMSetSourceFileName(module, ast->location.filename, strlen(ast->location.filename));

    LLVMBuilderRef builder = LLVMCreateBuilder();

    /*
    // TODO: too hard code
    LLVMTypeRef i32type = LLVMInt32TypeInContext(LLVMGetGlobalContext());
    LLVMTypeRef putchar_type = LLVMFunctionType(i32type, &i32type, 1, false);
    LLVMValueRef putchar_function = LLVMAddFunction(module, ast->data.declare.funcname, putchar_type);
    ast++;

    LLVMTypeRef main_type = LLVMFunctionType(i32type, NULL, 0, false);
    LLVMValueRef main_function = LLVMAddFunction(module, "main", main_type);

    LLVMBasicBlockRef block = LLVMAppendBasicBlockInContext(LLVMGetGlobalContext(), main_function, "block");
    LLVMPositionBuilderAtEnd(builder, block);

    for (; ast->type != AST_STMT_EOF; ast++)
    {
        LLVMValueRef arg = LLVMConstInt(i32type, ast->data.call.arg, false);
        LLVMBuildCall2(builder, putchar_type, putchar_function, &arg, 1, "putchar");
    }

    LLVMBuildRet(builder, LLVMConstInt(i32type, 0, false));
    */
    LLVMDisposeBuilder(builder);
    return module;
}
