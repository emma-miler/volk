#pragma once
#include "../expression_base.h"

namespace Volk
{
class ValueCastExpression : public ValueExpression
{
public:
    std::shared_ptr<ValueExpression> Value;
	std::function<void(ExpressionStack&)> CastFunc;

public:
    ValueCastExpression(std::shared_ptr<ValueExpression> expression, std::shared_ptr<VKType> targetType, std::function<void(ExpressionStack&)> func) : ValueExpression(ValueExpressionType::ValueCast, OperatorArity::Unary, expression->Token)
    {
        Value = expression;
		ResolvedType = targetType;
		CastFunc = func;
    }

    std::string ToString()
    {
        return fmt::format("ValueCastExpression(value='{}')", Value->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    void ToIR(ExpressionStack& stack);
	std::vector<std::shared_ptr<Expression>> SubExpressions();
    void ResolveNames(Scope* scope);
};
}
