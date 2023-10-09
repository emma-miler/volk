#include "parser.h"

namespace Volk
{

void VKParser::parserPass_NameResolution(Scope* scope)
{
    Log::TYPESYS->debug("Name resolution pass on scope '{}'", scope->ToString());
    for (auto&& func : scope->Functions)
    {
        parserPass_NameResolution(func.second->FunctionScope.get());
    }

    for (auto&& expr : scope->Expressions)
    {
        parserPass_NameResolution(expr.get(), scope);
    }

}

void VKParser::parserPass_NameResolution(Expression* expression, Scope* scope)
{
    expression->ResolveNames(scope);
    for (auto&& expr : expression->SubExpressions())
    {
        parserPass_NameResolution(expr, scope);
    }
}
}
