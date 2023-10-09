#include "../../expression/value_valuecast.h"

namespace Volk
{
std::string ValueCastExpression::ToHumanReadableString(std::string depthPrefix)
{
	std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "ValueCastExpression";
    if (ResolvedType != nullptr)
    {
        out += newline + fmt::format("type={}", ResolvedType->Name);
    }
    out += newline + fmt::format("value={}", Value->ToHumanReadableString(depthPrefix + INDENT));
    return  out;
}

void ValueCastExpression::ToIR(ExpressionStack& stack)
{
    Value->ToIR(stack);
    stack.Comment("START VALUE CAST\n");
	CastFunc(stack);
	stack.Comment("END VALUE CAST\n");
}

void ValueCastExpression::ResolveNames(Scope* scope)
{
    Value->ResolveNames(scope);
}


std::vector<std::shared_ptr<Expression>> ValueCastExpression::SubExpressions()
{
    return std::vector<std::shared_ptr<Expression>>{ Value };
}
}
