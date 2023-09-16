#pragma once
#include "../expression_base.h"

namespace Volk
{
class IndirectValueExpression : public ValueExpression
{
public:
    std::string Value;

    std::shared_ptr<Volk::Variable> ResolvedVariable;

public:
    IndirectValueExpression(std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::Indirect, OperatorArity::Nullary, token)
    {
        Value = token->Value;
    }

    std::string ToString()
    {
        return fmt::format("IndirectValueExpression(value='{}')", Value);
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    void ToIR(ExpressionStack& stack);
    void ResolveNames(Scope* scope);
};
}
