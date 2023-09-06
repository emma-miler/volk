#include "parser.h"

namespace Volk
{

void VKParser::parserPass_TypeChecking(Expression* expression, Scope* scope)
{
    for (auto&& subexpr : expression->SubExpressions())
    {
        parserPass_TypeChecking(subexpr, scope);
    }
    expression->TypeCheck(scope);
}
}
