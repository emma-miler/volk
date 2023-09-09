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

    std::string ToHumanReadableString(std::string depthPrefix)
    {
        std::string newline = fmt::format("\n{}\t", depthPrefix);
        std::string out = "FunctionDeclarationExpression(";
        out += newline + fmt::format("value='{}'", Function->ToString());
        out += "\n" + depthPrefix + ")";
        return  out;
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        stack.Comment("FUNCTION DECLARATION");
    }

    virtual void ResolveNames(Scope* scope)
    {
        for (auto&& expr : Function->FunctionScope->Expressions)
        {
            expr->ResolveNames(Function->FunctionScope.get());
        }
    }
};
}
