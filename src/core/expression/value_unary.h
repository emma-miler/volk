#pragma once
#include "../expression_base.h"

namespace Volk
{
class UnaryValueExpression : public ValueExpression
{
public:
    std::unique_ptr<Expression> Value;
    OperatorType Operator;

public:
    UnaryValueExpression(OperatorType op, std::unique_ptr<Expression> value, std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::Unary, OperatorArity::Unary, token)
    {
        Value = std::move(value);
        Operator = op;
    }

    std::string ToString()
    {
        return fmt::format("UnaryValueExpression(op={}, value={})", OperatorTypeNames[Operator], Value->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
         return fmt::format("UnaryValueExpression(\n{}\top={}, \n{}\tvalue={}\n{})", depthPrefix, OperatorTypeNames[Operator], depthPrefix, Value->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        Value->ToIR(stack);
        std::string valueVariableName = stack.ActiveVariable.Name;
        // Perform the operator
        stack.Comment("START UNARY OPERATOR");
        if (stack.ActiveVariable.IsPointer)
        {
            stack.AdvanceActive(0);
            stack.Expressions.push_back(fmt::format("%{} = load i64, ptr %{}", stack.ActiveVariable.Name, valueVariableName));
            valueVariableName = stack.ActiveVariable.Name;
        }
        stack.AdvanceActive(0);
        stack.Expressions.push_back(fmt::format("%{} = {} nsw i64 0, %{}", stack.ActiveVariable.Name, Operator == OperatorType::OperatorMinus ? "sub" : "add", valueVariableName));
        stack.Comment("END UNARY OPERATOR\n");
    }

    std::vector<Expression*> SubExpressions()
    {
        return std::vector<Expression*>{ Value.get() };
    }

    virtual void TypeCheck(Scope* scope)
    {
        // TODO: need to check if type supports operation
        return;
    }
};
}
