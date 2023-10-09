#pragma once
#include "../expression_base.h"

namespace Volk
{
class StringConstantValueExpression : public ValueExpression
{
public:
    std::string Index;

public:
    StringConstantValueExpression(std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::StringConstant, OperatorArity::Nullary, token)
    {
        Index = token->Value;
    }

    std::string ToString()
    {
        return fmt::format("StringConstantValueExpression(index='{}')", Index);
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    void ToIR(ExpressionStack& stack);
};
}
