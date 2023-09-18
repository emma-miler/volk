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
                              i++,
                              entry.length() + 1,
                              string_as_llvm_string(entry) + "\\00") << std::endl;
    }

    ExpressionStack funcStack;
    funcStack.NameCounter = 0;

    for (auto&& func : program.DefaultScope->Functions)
    {
        func.second->ToIR(funcStack);
    }
    for (auto&& line : funcStack.Expressions)
    {
        output << line << std::endl;
    }

    output << "define dso_local noundef i64 @main() {\n";
    output << "entry:\n";
    ExpressionStack stack;
    stack.NameCounter = 0;
    for (auto&& expr : program.DefaultScope->Expressions)
    {
        expr->ToIR(stack);
    }

    for (std::string& line : stack.Expressions)
    {
        output << line << std::endl;
    }

    output << "}\n";
    // Builtin printf
    output << "declare i64 @printf(ptr noundef, ...) #99\n";

    return output.str();
}
}
