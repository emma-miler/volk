#pragma once
#include "../expression_base.h"

namespace Volk
{

class ReturnExpression : public Expression
{
public:
    std::unique_ptr<ValueExpression> Value;
    std::shared_ptr<VKType> ReturnType;

public:
    ReturnExpression(std::unique_ptr<ValueExpression> value, std::shared_ptr<VKType> returnType, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::ReturnExpr, token)
    {
        // TODO: check if return type matches return type of function
        Value = std::move(value);
        ReturnType = returnType;
    }

  public:
    std::string ToString()
    {
        return fmt::format("ReturnExpression(value='{}')", Value->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
         return fmt::format("ReturnExpression(\n{}\tvalue={}\n{})", depthPrefix, Value->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
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

    std::vector<Expression*> SubExpressions()
    {
        return std::vector<Expression*>{ Value.get() };
    }

    virtual void TypeCheck(Scope* scope)
    {
        if (Value->ResolvedType != ReturnType)
        {
            Log::TYPESYS->error("Cannot implicitly convert from '{}' to '{}'", Value->ResolvedType->Name, ReturnType->Name);
            Token->Indicate();
            throw type_error("");
        }
    }

};

}
