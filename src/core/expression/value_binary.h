#pragma once
#include "../expression_base.h"

namespace Volk
{
class BinaryValueExpression : public ValueExpression
{
public:
    std::unique_ptr<ValueExpression> Left;
    std::unique_ptr<ValueExpression> Right;
    OperatorType Operator;

public:
    BinaryValueExpression(OperatorType op, std::unique_ptr<ValueExpression> left, std::unique_ptr<ValueExpression> right, std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::Binary, OperatorArity::Binary, token)
    {
        Left = std::move(left);
        Right = std::move(right);
        Operator = op;
    }

    std::string ToString()
    {
        return fmt::format("BinaryValueExpression(op={}, left={}, right={})",
                           OperatorTypeNames[Operator], Left->ToString(), Right->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    virtual void ToIR(ExpressionStack& stack);
    std::vector<Expression*> SubExpressions();

    void TypeCheck(Scope* scope);

};
}
