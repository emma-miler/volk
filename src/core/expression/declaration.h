#pragma once
#include "../expression_base.h"

namespace Volk
{
class DeclarationExpression : public Expression
{
public:
    std::string Typename;
    std::string Name;


    std::shared_ptr<VKType> ResolvedType;

public:
    DeclarationExpression(std::string type, std::string name, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::Declaration, token)
    {
        Typename = type;
        Name = name;
    }
public:
    std::string ToString()
    {
        return fmt::format("DeclarationExpression(type='{}', name='{}')", Typename, Name);
    }

    std::string ToHumanReadableString(std::string depthPrefix);
    void ToIR(ExpressionStack& stack);
    void ResolveNames(Scope* scope);
};
}
