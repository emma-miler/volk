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
        parserPass_NameResolution(expr, scope);
    }
}

void VKParser::parserPass_NameResolution(std::shared_ptr<Expression> expression, Scope* scope)
{
    expression->ResolveNames(scope);
}
}
