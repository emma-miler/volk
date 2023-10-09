#pragma once
#include "../expression_base.h"

#include "../expression/assignment.h"

namespace Volk
{
class ForExpression : public Expression
{
public:
    std::shared_ptr<AssignmentExpression> Initializer;
    std::shared_ptr<ValueExpression> Condition;
    std::shared_ptr<Expression> Increment;

    std::shared_ptr<Scope> InnerScope;

public:
    ForExpression(std::shared_ptr<AssignmentExpression> initializer, std::shared_ptr<ValueExpression> condition, std::shared_ptr<Expression> increment, std::shared_ptr<Scope> innerScope, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::For, token)
    {
        Initializer = initializer;
        Condition = condition;
        Increment = increment;
        InnerScope = innerScope;
    }

public:
    std::string ToString()
    {
        return fmt::format("ForExpression()");
    }
    std::string ToHumanReadableString(std::string depthPrefix);

    std::vector<std::shared_ptr<Expression>> SubExpressions();

    void ToIR(ExpressionStack& stack);
    void ResolveNames(Scope* scope);
    void TypeCheck(Scope* scope);
};
}
