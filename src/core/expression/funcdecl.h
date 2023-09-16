#pragma once
#include "../expression_base.h"

namespace Volk
{
class FunctionDeclarationExpression : public Expression
{
public:
    std::shared_ptr<FunctionObject> Function;

public:
    FunctionDeclarationExpression(std::shared_ptr<FunctionObject> function, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::FunctionDeclaration, token)
    {
        Function = function;
    }
public:
    std::string ToString()
    {
        return fmt::format("FunctionDeclarationExpression(name='{}')", Function->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix);

    void ToIR(ExpressionStack& stack)
    {
        stack.Comment("FUNCTION DECLARATION");
    }

    void ResolveNames(Scope* scope);
};
}
