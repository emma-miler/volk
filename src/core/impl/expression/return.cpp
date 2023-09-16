#include "../../expression/return.h"

namespace Volk
{

std::string ReturnExpression::ToHumanReadableString(std::string depthPrefix)
{
        return fmt::format("ReturnExpression(\n{}\tvalue={}\n{})", depthPrefix, Value->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
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
        stack.Expressions.push_back(fmt::format("%{} = load {}, {}", stack.ActiveVariable.Name, ReturnType->LLVMType, variable.Get()));
        stack.Expressions.push_back(fmt::format("ret {} %{}",ReturnType->LLVMType, stack.ActiveVariable.Name));
    }
    else
    {
        stack.Expressions.push_back(fmt::format("ret {} %{}", ReturnType->LLVMType, stack.ActiveVariable.Name));
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
