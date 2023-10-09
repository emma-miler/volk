#pragma once
#include "../expression_base.h"

namespace Volk
{
class UnaryValueExpression : public ValueExpression
{
public:
    std::shared_ptr<ValueExpression> Value;
    OperatorType Operator;

public:
    UnaryValueExpression(OperatorType op, std::shared_ptr<ValueExpression> value, std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::Unary, OperatorArity::Unary, token)
    {
        Value = value;
        Operator = op;
		ResolvedType = Value->ResolvedType;
    }

    std::string ToString()
    {
        return fmt::format("UnaryValueExpression(op={}, value={})", OperatorTypeNames[Operator], Value->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    void ToIR(ExpressionStack& stack);
    std::vector<Expression*> SubExpressions();
    virtual void TypeCheck(Scope* scope);
};
}
