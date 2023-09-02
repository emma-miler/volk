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

    std::string ToHumanReadableString(std::string depthPrefix)
    {
        return fmt::format("StringConstantValueExpression(index={})", Index);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        // We dont want to actually advance the counter here,
        // since we're not actually pushing anything new
        stack.AdvanceActive(1);
        stack.ActiveVariable.IsConstant = 1;
        stack.ActiveVariable.Name = fmt::format(".str.{}", Index);
        stack.NameCounter--;
        stack.Comment("PUSHED STRING CONSTANT VALUE");
    }
};
}
