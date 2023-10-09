#pragma once
#include "../expression_base.h"
#include "../token/value.h"

namespace Volk
{
class ImmediateValueExpression : public ValueExpression
{
public:
    std::string Value;

public:
    ImmediateValueExpression(std::shared_ptr<ValueToken> token) : ValueExpression(ValueExpressionType::Immediate, OperatorArity::Nullary, token)
    {
        Value = token->Value;
        ResolvedType = token->ValueType;
    }

    std::string ToString()
    {
        return fmt::format("ImmediateValueExpression(value='{}')", Value);
    }

    std::string ToHumanReadableString(std::string depthPrefix);

    void ToIR(ExpressionStack& stack);
};

}
