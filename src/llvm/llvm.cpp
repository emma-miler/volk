#include "llvm.h"

#include <sstream>

namespace Volk
{
std::string VKLLVM::generateOutput(VKParser& parser)
{
    std::stringstream output;


    for (auto&& func : parser.RootNamespace->Functions)
    {
        ExpressionStack stack;
        output << func->ToIR();
        output << " #0\n{" << std::endl;
        for (auto&& expr : func->FunctionScope->Expressions)
        {
            expr->ToIR(stack);
        }
        for (std::string& line : stack.Expressions)
        {
            output << "\t" << line << std::endl;
        }
        output << "}" << std::endl;;
    }

    ExpressionStack stack;
    stack.NameCounter = 0;

    output << "define dso_local noundef i32 @main() {\n";
    output << "entry:\n";
    for (auto&& expr : parser.DefaultScope->Expressions)
    {
        expr->ToIR(stack);
    }

    for (std::string& line : stack.Expressions)
    {
        output << "\t" << line << std::endl;
    }

    output << "}\n";
    // Builtin printf
    output << "declare i32 @printf(ptr noundef, ...) #99\n";

    return output.str();
}
}
