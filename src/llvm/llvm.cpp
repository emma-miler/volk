#include "llvm.h"

#include <sstream>

namespace Volk
{
std::string VKLLVM::generateOutput(VKParser& parser)
{
    std::stringstream output;
    output << "define dso_local i32 @main() {\n";
    output << "entry:\n";

    ExpressionStack stack;

    for (auto&& expr : parser.Expressions)
    {
        expr->ToIR(stack);
    }

    for (std::string& line : stack.Expressions)
    {
        output << "\t" << line << std::endl;
    }

    output << "}\n";
    // Builtin printf
    output << "declare i32 @printf(ptr noundef, ...) #2\n";

    return output.str();
}
}
