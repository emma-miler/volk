#pragma once
#include "../expression_base.h"
#include "value_immediate.h"

#include "../variable.h"
#include "../scope.h"

namespace Volk
{
class AssignmentExpression : public Expression
{
public:
    std::string Name;
    std::shared_ptr<ValueExpression> Value;

    std::shared_ptr<Variable> ResolvedVariable;

public:
    AssignmentExpression(std::string name, std::shared_ptr<ValueExpression> value, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::Assignment, token)
    {
        Name = name;
        Value = value;
    }
public:
    std::string ToString()
    {
        return fmt::format("AssignmentExpression(name='{}', value={})", Name, Value->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix);

    void ToIR(ExpressionStack& stack);
    std::vector<std::shared_ptr<Expression>> SubExpressions();
    void ResolveNames(Scope* scope);

    void TypeCheck(Scope* scope);
};
}
