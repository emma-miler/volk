#include "../../expression/declaration.h"

namespace Volk
{
std::string DeclarationExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "DeclarationExpression";
    out += newline + fmt::format("type={}", Typename);
    out += newline + fmt::format("value={}", Name);
    return  out;
}

void DeclarationExpression::ToIR(ExpressionStack& stack)
{
    stack.Comment("START DECLARATION");
    stack.Operation(fmt::format("%{} = alloca {}", Name, ResolvedType->LLVMType));
    stack.Comment("END DECLARATION\n");
}

void DeclarationExpression::ResolveNames(Scope* scope)
{
    ResolvedType = scope->FindType(Typename);
    if (ResolvedType == nullptr)
    {
        Log::TYPESYS->error("Unknown variable '{}'", Name);
        throw type_error("");
    }
}
}
