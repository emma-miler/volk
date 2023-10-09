#pragma once
#include "../expression_base.h"

namespace Volk
{

class IfStatementExpression : public Expression
{
public:
    std::shared_ptr<ValueExpression> Condition;
    std::shared_ptr<Scope> ScopeIfTrue;
    std::shared_ptr<Scope> ScopeIfFalse;
    bool HasElseClauseDefined;

public:
    IfStatementExpression(std::shared_ptr<ValueExpression> condition, std::shared_ptr<Scope> parentScope, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::IfStatement, token)
    {
        // TODO: check if return type matches return type of function
        Condition = condition;
        ScopeIfTrue = std::make_shared<Scope>("__impl_if_true", parentScope, parentScope->ReturnType);
        ScopeIfFalse = std::make_shared<Scope>("__impl_if_false", parentScope, parentScope->ReturnType);
    }

  public:
    std::string ToString()
    {
        return fmt::format("IfStatementExpression(value='{}')", Condition->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    virtual void ToIR(ExpressionStack& stack);

    std::vector<Expression*> SubExpressions();

    virtual void TypeCheck(Scope* scope);
};

}
