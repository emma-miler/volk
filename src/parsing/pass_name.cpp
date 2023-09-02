#include "parser.h"

namespace Volk
{

void VKParser::parserPass_NameResolution(Expression* expression, Scope* scope)
{
    expression->ResolveNames(scope);
    for (auto&& expr : expression->SubExpressions())
    {
        parserPass_NameResolution(expr, scope);
    }
}
}
