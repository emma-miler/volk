#include "../../expression/value_immediate.h"

namespace Volk
{
std::string ImmediateValueExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "ImmediateValueExpression";
    if (ResolvedType != nullptr)
    {
        out += newline + fmt::format("type={}", ResolvedType->Name);
    }
    out += newline + fmt::format("value={}", Value);
    return  out;
}

void ImmediateValueExpression::ToIR(ExpressionStack& stack)
{
    stack.AdvanceActive(1);
    // Allocate the variable
    stack.Comment("START IMMEDIATE VALUE");
    stack.Operation(fmt::format("%{} = alloca {}", stack.ActiveVariable.Name, ResolvedType->LLVMType));
    // Assign the value
    stack.Operation(fmt::format("store {} {}, {}", ResolvedType->LLVMType, Value, stack.ActiveVariable.Get()));
    stack.Comment("END IMMEDIATE VALUE\n");
}
}
