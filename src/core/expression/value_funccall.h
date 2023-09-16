#pragma once
#include "../expression_base.h"
#include "../function.h"

namespace Volk
{

class FunctionCallValueExpression : public ValueExpression
{
public:
    std::string FunctionName;
    std::vector<std::unique_ptr<ValueExpression>> Arguments;


    std::shared_ptr<FunctionObject> ResolvedFunction;

public:
    FunctionCallValueExpression(std::string functionName, std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::FunctionCall, OperatorArity::Nullary, token)
    {
        FunctionName = functionName;
    }

    std::string ToString()
    {
        return fmt::format("FunctionCallValueExpression(name='{}')", FunctionName);
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    virtual void ToIR(ExpressionStack& stack);
    void ResolveNames(Scope* scope);

    std::vector<Expression*> SubExpressions();

    void TypeCheck(Scope* scope);

};

}
