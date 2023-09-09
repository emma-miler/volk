#include "parser.h"

namespace Volk
{

void VKParser::parserPass_TypeChecking(Scope* scope)
{
    Log::TYPESYS->debug("Type resolution pass on scope '{}'", scope->ToString());
    for (auto&& func : scope->Functions)
    {
        parserPass_TypeChecking(func.second->FunctionScope.get());
    }
    for (auto&& subexpr : scope->Expressions)
    {
        parserPass_TypeChecking(subexpr.get(), scope);
    }
}

void VKParser::parserPass_TypeChecking(Expression* expression, Scope* scope)
{
    for (auto&& subexpr : expression->SubExpressions())
    {
        parserPass_TypeChecking(subexpr, scope);
    }
    expression->TypeCheck(scope);
}
}
