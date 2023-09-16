#pragma once
#include "../expression_base.h"

namespace Volk
{
class ValueCastExpression : public ValueExpression
{
public:
    std::unique_ptr<ValueExpression> Value;
	std::function<void(ExpressionStack&)> CastFunc;

public:
    ValueCastExpression(std::unique_ptr<ValueExpression> expression, std::shared_ptr<VKType> targetType, std::function<void(ExpressionStack&)> func) : ValueExpression(ValueExpressionType::ValueCast, OperatorArity::Unary, expression->Token)
    {
        Value = std::move(expression);
		ResolvedType = targetType;
		CastFunc = func;
    }

    std::string ToString()
    {
        return fmt::format("ValueCastExpression(value='{}')", Value->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    void ToIR(ExpressionStack& stack);
	std::vector<Expression*> SubExpressions();
};
}
