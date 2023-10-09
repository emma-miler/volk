#include "../../expression/return.h"

namespace Volk
{

std::string ReturnExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "ReturnExpression";
    if (Value != nullptr)
        out += newline + fmt::format("value={}", Value->ToHumanReadableString(depthPrefix + INDENT));
    return out;
}

void ReturnExpression::ToIR(ExpressionStack& stack)
{
    stack.Comment("START RETURN VALUE");
    if (Value != nullptr)
    {
        Value->ToIR(stack);

        stack.Comment("END RETURN VALUE");
        stack.Comment("START RETURN");
        if (stack.ActiveVariable.IsPointer)
        {
            IRVariableDescriptor variable = stack.ActiveVariable;
            stack.AdvanceActive(0);
            stack.Operation("%{} = load {}, {}", stack.ActiveVariable.Name, ReturnType->LLVMType, variable.Get());
            stack.Operation("ret {} %{}",ReturnType->LLVMType, stack.ActiveVariable.Name);
        }
        else
        {
            stack.Operation("ret {} %{}", ReturnType->LLVMType, stack.ActiveVariable.Name);
        }
    }
    else
    {
        stack.Operation("ret void");
    }
    stack.Comment("END RETURN\n");
}

std::vector<std::shared_ptr<Expression>> ReturnExpression::SubExpressions()
{
    if (Value != nullptr)
        return std::vector<std::shared_ptr<Expression>>{ Value };
    else
        return std::vector<std::shared_ptr<Expression>>();
}

void ReturnExpression::ResolveNames(Scope* scope)
{
    if (Value != nullptr)
        Value->ResolveNames(scope);
}


void ReturnExpression::TypeCheck(Scope* scope)
{
    if (Value == nullptr)
    {
        if (ReturnType != BUILTIN_VOID)
        {
            Log::TYPESYS->error("Cannot implicitly convert from '__null__' to '{}'", ReturnType->Name);
            Token->Indicate();
            throw type_error("");
        }
        return;
    }
    Value->TypeCheck(scope);
    if (Value->ResolvedType != ReturnType)
    {

        Log::TYPESYS->error("Cannot implicitly convert from '{}' to '{}'", Value->ResolvedType->Name, ReturnType->Name);
        Token->Indicate();
        throw type_error("");
    }
}

}
