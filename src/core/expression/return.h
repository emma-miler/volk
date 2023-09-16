#pragma once
#include "../expression_base.h"

namespace Volk
{

class ReturnExpression : public Expression
{
public:
    std::unique_ptr<ValueExpression> Value;
    std::shared_ptr<VKType> ReturnType;

public:
    ReturnExpression(std::unique_ptr<ValueExpression> value, std::shared_ptr<VKType> returnType, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::ReturnExpr, token)
    {
        // TODO: check if return type matches return type of function
        Value = std::move(value);
        ReturnType = returnType;
    }

  public:
    std::string ToString()
    {
        return fmt::format("ReturnExpression(value='{}')", Value->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    virtual void ToIR(ExpressionStack& stack);

    std::vector<Expression*> SubExpressions();

    virtual void TypeCheck(Scope* scope);

};

}
