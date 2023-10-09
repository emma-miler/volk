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
    for (auto&& expr : scope->Expressions)
    {
        parserPass_TypeChecking(expr, scope);
    }
}

void VKParser::parserPass_TypeChecking(std::shared_ptr<Expression> expression, Scope* scope)
{
    expression->TypeCheck(scope);
}
}
