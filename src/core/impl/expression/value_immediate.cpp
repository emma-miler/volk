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
    //stack.AdvanceActive(0);
	stack.ActiveVariable.Name = Value;
	stack.ActiveVariable.VarType = IRVarType::Immediate;
	stack.ActiveVariable.Type = ResolvedType->LLVMType;
    // Allocate the variable
    //stack.Comment("START IMMEDIATE VALUE");
    //stack.Operation("%{} = alloca {}", stack.ActiveVariable.Name, ResolvedType->LLVMType);
    // Assign the value
    //stack.Operation("store {} {}, {}", ResolvedType->LLVMType, Value, stack.ActiveVariable.Get());
    //stack.Comment("END IMMEDIATE VALUE\n");
}
}
