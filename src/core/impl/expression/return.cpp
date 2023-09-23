#include "../../expression/return.h"

namespace Volk
{

std::string ReturnExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "ReturnExpression";
    out += newline + fmt::format("value={}", Value->ToHumanReadableString(depthPrefix + INDENT));
    return out;
}

void ReturnExpression::ToIR(ExpressionStack& stack)
{
    stack.Comment("START RETURN VALUE");
    Value->ToIR(stack);
    stack.Comment("END RETURN VALUE");
    stack.Comment("START RETURN");
    if (stack.ActiveVariable.IsPointer)
    {
        IRVariableDescriptor variable = stack.ActiveVariable;
        stack.AdvanceActive(0);
        stack.Operation(fmt::format("%{} = load {}, {}", stack.ActiveVariable.Name, ReturnType->LLVMType, variable.Get()));
        stack.Operation(fmt::format("ret {} %{}",ReturnType->LLVMType, stack.ActiveVariable.Name));
    }
    else
    {
        stack.Operation(fmt::format("ret {} %{}", ReturnType->LLVMType, stack.ActiveVariable.Name));
    }
    stack.Comment("END RETURN\n");
}

std::vector<Expression*> ReturnExpression::SubExpressions()
{
    return std::vector<Expression*>{ Value.get() };
}

void ReturnExpression::TypeCheck(Scope* scope)
{
    if (Value->ResolvedType != ReturnType)
    {
        Log::TYPESYS->error("Cannot implicitly convert from '{}' to '{}'", Value->ResolvedType->Name, ReturnType->Name);
        Token->Indicate();
        throw type_error("");
    }
}

}
