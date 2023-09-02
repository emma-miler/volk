#include "parser.h"

namespace Volk
{

void VKParser::parserPass_TypeChecking(Expression* expression, Scope* scope)
{
    for (auto&& expr : expression->SubExpressions())
    {
        parserPass_TypeChecking(expr, scope);
    }
    if (expression->Type == ExpressionType::Declaration)
    {
        DeclarationExpression* expr = (DeclarationExpression*)expression;
        expr->ResolvedType = scope->FindType(expr->Typename);
        if (expr->ResolvedType == nullptr)
        {
            Log::TYPESYS->error("Unknown type '{}'", expr->Typename);
            expr->Token->Indicate();
            throw type_error("");
        }
    }
    if (expression->Type == ExpressionType::Value)
    {
        ValueExpression* valueExpression = (ValueExpression*)expression;
        if (valueExpression->ValueExpressionType == ValueExpressionType::Indirect)
        {
            IndirectValueExpression* expr = (IndirectValueExpression*)expression;
            expr->ResolvedVariable = scope->FindVariable(expr->Value);

             if (expr->ResolvedVariable == nullptr)
            {
                Log::TYPESYS->error("Unknown variable '{}'", expr->Value);
                expr->Token->Indicate();
                throw type_error("");
            }

            expr->ResolvedType = expr->ResolvedVariable->Type;
        }
        if (valueExpression->ValueExpressionType == ValueExpressionType::FunctionCall)
        {
            FunctionCallValueExpression* expr = (FunctionCallValueExpression*)expression;
            if (scope->FindVariable(expr->FunctionName) == nullptr)
            {
                Log::TYPESYS->error("Unknown function '{}'", expr->FunctionName);
                expr->Token->Indicate();
                throw type_error("");
            }
        }
        else if (valueExpression->ValueExpressionType == ValueExpressionType::Binary)
        {
            BinaryValueExpression* expr = (BinaryValueExpression*)expression;
            Log::PARSER->trace("Left: {}", expr->Left->ResolvedType->ToString());
            Log::PARSER->trace("Right: {}", expr->Right->ResolvedType->ToString());
            if (expr->Left->ResolvedType != expr->Right->ResolvedType)
            {
                Log::TYPESYS->error("Mismatched types '{}' and '{}' for operator '{}'", expr->Left->ResolvedType->ToString(), expr->Right->ResolvedType->ToString(), OperatorTypeNames[expr->Operator]);
                expr->Token->Indicate();
                throw type_error("");
            }
        }
    }
}
}
