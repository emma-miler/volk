#include "../../expression/funcdecl.h"

namespace Volk
{

std::string FunctionDeclarationExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}\t", depthPrefix);
    std::string out = "FunctionDeclarationExpression(";
    out += newline + fmt::format("value='{}'", Function->ToString());
    out += "\n" + depthPrefix + ")";
    return  out;
}

void FunctionDeclarationExpression::ResolveNames(Scope* scope)
{
    for (auto&& expr : Function->FunctionScope->Expressions)
    {
        expr->ResolveNames(Function->FunctionScope.get());
    }
}
}
