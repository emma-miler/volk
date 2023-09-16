#include "../../expression/value_immediate.h"

namespace Volk
{
std::string ImmediateValueExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}\t", depthPrefix);
    std::string out = "ImmediateValueExpression(";
    if (ResolvedType != nullptr)
    {
        out += newline + fmt::format("type='{}'", ResolvedType->Name);
    }
    out += newline + fmt::format("value='{}'", Value);
    out += "\n" + depthPrefix + ")";
    return  out;
}

void ImmediateValueExpression::ToIR(ExpressionStack& stack)
{
    stack.AdvanceActive(1);
    // Allocate the variable
    stack.Comment("START IMMEDIATE VALUE");
    stack.Expressions.push_back(fmt::format("%{} = alloca i64", stack.ActiveVariable.Name));
    // Assign the value
    stack.Expressions.push_back(fmt::format("store i64 {}, {}", Value, stack.ActiveVariable.Get()));
    stack.Comment("END IMMEDIATE VALUE\n");
}
}
