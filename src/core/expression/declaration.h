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

    virtual void ToIR(ExpressionStack& stack)
    {
        stack.Comment("START DECLARATION");
        stack.Expressions.push_back(fmt::format("%{} = alloca {}", Name, ResolvedType->LLVMType));
        stack.Comment("END DECLARATION\n");
    }

    virtual void ResolveNames(Scope* scope)
    {
        ResolvedType = scope->FindType(Typename);
        if (ResolvedType == nullptr)
        {
            Log::TYPESYS->error("Unknown variable '{}'", Name);
            throw type_error("");
        }
    }
};
}
