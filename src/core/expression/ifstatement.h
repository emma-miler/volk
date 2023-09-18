#pragma once
#include "../expression_base.h"

namespace Volk
{

class IfStatementExpression : public Expression
{
public:
    std::shared_ptr<ValueExpression> Condition;
    std::shared_ptr<Scope> InnerScope;

public:
    IfStatementExpression(std::shared_ptr<ValueExpression> condition, std::shared_ptr<Scope> parentScope, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::IfStatement, token)
    {
        // TODO: check if return type matches return type of function
        Condition = condition;
        InnerScope = std::make_shared<Scope>("test", parentScope, BUILTIN_VOID);
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
