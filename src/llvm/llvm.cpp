#include "llvm.h"

#include <sstream>

#include "../util/string.h"

namespace Volk
{
std::string VKLLVM::generateOutput(Program& program)
{
    std::stringstream output;

    int i = 0;
    for (auto&& entry : program.StringTable)
    {
        output << fmt::format("@.str.{} = private unnamed_addr constant [{} x i8] c\"{}\", align 1",
                              i,
                              entry.length() + 1,
                              string_as_llvm_string(entry) + "\\00") << std::endl;
    }


    for (auto&& func : program.RootNamespace->Functions)
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
    for (auto&& expr : program.DefaultScope->Expressions)
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
