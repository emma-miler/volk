#include "../../expression/value_unary.h"

namespace Volk
{
std::string UnaryValueExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "UnaryValueExpression";
    out += newline + fmt::format("op={}", OperatorTypeNames[Operator]);
    out += newline + fmt::format("value={}", Value->ToHumanReadableString(depthPrefix + INDENT));
    return out;
}

void UnaryValueExpression::ToIR(ExpressionStack& stack)
{
    Value->ToIR(stack);
    std::string valueVariableName = stack.ActiveVariable.Name;
    // Perform the operator
    stack.Comment("START UNARY OPERATOR");
    if (stack.ActiveVariable.IsPointer)
    {
        stack.AdvanceActive(0);
        stack.Operation(fmt::format("%{} = load i64, ptr %{}", stack.ActiveVariable.Name, valueVariableName));
        valueVariableName = stack.ActiveVariable.Name;
    }
    stack.AdvanceActive(0);
    stack.Operation(fmt::format("%{} = {} nsw i64 0, %{}", stack.ActiveVariable.Name, Operator == OperatorType::OperatorMinus ? "sub" : "add", valueVariableName));
    stack.Comment("END UNARY OPERATOR\n");
}

std::vector<Expression*> UnaryValueExpression::SubExpressions()
{
    return std::vector<Expression*>{ Value.get() };
}

void UnaryValueExpression::TypeCheck(Scope* scope)
{
    // TODO: need to check if type supports operation
    return;
}
}
