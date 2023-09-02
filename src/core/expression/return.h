#pragma once
#include "../expression_base.h"

namespace Volk
{

class ReturnExpression : public Expression
{
public:
    std::unique_ptr<ValueExpression> Value;

public:
    ReturnExpression(std::unique_ptr<ValueExpression> value, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::ReturnExpr, token)
    {
        Value = std::move(value);
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
            stack.Expressions.push_back(fmt::format("%{} = load i64, {}", stack.ActiveVariable.Name, variable.Get()));
            stack.Expressions.push_back(fmt::format("ret i64 %{}", stack.ActiveVariable.Name));
        }
        else
        {
            stack.Expressions.push_back(fmt::format("ret i64 %{}", stack.ActiveVariable.Name));
        }
        stack.Comment("END RETURN\n");
    }

    std::vector<Expression*> SubExpressions()
    {
        return std::vector<Expression*>{ Value.get() };
    }
};

}
