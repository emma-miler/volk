#pragma once
#include "../expression_base.h"
#include "../token/operator.h"

namespace Volk
{
class BinaryValueExpression : public ValueExpression
{
public:
    std::shared_ptr<ValueExpression> Left;
    std::shared_ptr<ValueExpression> Right;
    OperatorType Operator;
    bool IsComparator;
    ComparisonFunction CompareFunction;

public:
    BinaryValueExpression(OperatorType op, std::shared_ptr<ValueExpression> left, std::shared_ptr<ValueExpression> right, std::shared_ptr<OperatorToken> token) : ValueExpression(ValueExpressionType::Binary, OperatorArity::Binary, token)
    {
        Left = left;
        Right = right;
        Operator = op;
        IsComparator = token->IsComparator;
    }

    std::string ToString()
    {
        return fmt::format("BinaryValueExpression(op={}, left={}, right={})",
                           OperatorTypeNames[Operator], Left->ToString(), Right->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    virtual void ToIR(ExpressionStack& stack);
    std::vector<std::shared_ptr<Expression>> SubExpressions();

    void ResolveNames(Scope* scope);
    void TypeCheck(Scope* scope);

};
}
