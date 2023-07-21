#include "llvm.h"

#include <sstream>

namespace Volk
{
std::string VKLLVM::generateOutput(VKParser& parser)
{
    std::stringstream output;
    output << "define dso_local i32 @main() {\n";
    output << "entry:\n";

    for (auto&& expr : parser.Expressions)
    {
        output << "\t" << expr->ToIR() << std::endl;
    }

    output << "}\n";
    // Builtin printf
    output << "declare i32 @printf(ptr noundef, ...) #2\n";

    return output.str();
}
}
