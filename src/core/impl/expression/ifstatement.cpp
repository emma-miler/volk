#include "../../expression/ifstatement.h"

namespace Volk
{

std::string IfStatementExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "IfStatementExpression";
    out += newline + fmt::format("condition={}", Condition->ToHumanReadableString(depthPrefix + INDENT + INDENT));
    out += newline + fmt::format("if_true={}", Condition->ToHumanReadableString(depthPrefix + INDENT));
    for (auto&& statement : ScopeIfTrue->Expressions)
    {
        out += newline + fmt::format("{}{},", depthPrefix, statement->ToHumanReadableString(depthPrefix + INDENT + INDENT));
    }
    out = out.substr(0, out.size() - 1);
    out += newline + fmt::format("if_false={}", Condition->ToHumanReadableString(depthPrefix + INDENT));
    for (auto&& statement : ScopeIfFalse->Expressions)
    {
        out += newline + fmt::format("{}{},", depthPrefix, statement->ToHumanReadableString(depthPrefix + INDENT + INDENT));
    }
    out = out.substr(0, out.size() - 1);
    return  out;
}

void IfStatementExpression::ToIR(ExpressionStack& stack)
{
    stack.Comment("START CONDITION VALUE");
    Condition->ToIR(stack);
    stack.Comment("END CONDITION VALUE");
    stack.Comment("START RETURN");
    IRVariableDescriptor conditionVar = stack.ActiveVariable;
    if (stack.ActiveVariable.IsPointer)
    {
        stack.AdvanceActive(0);
        stack.Operation(fmt::format("%{} = load i8, {}", stack.ActiveVariable.Name, conditionVar.Get()));
        conditionVar = stack.ActiveVariable;
    }
    if (stack.ActiveVariable.Type != "i1")
    {
        stack.AdvanceActive(0);
        stack.Operation(fmt::format("{} = trunc {} to i1", stack.ActiveVariable.GetOnlyName(), conditionVar.Get()));
    }
    std::string branchSuffix = stack.ActiveVariable.Name;
    stack.Operation(fmt::format("br i1 {}, label %if{}.then, label %if{}.else", stack.ActiveVariable.GetOnlyName(), branchSuffix, branchSuffix));
    stack.Label(fmt::format("if{}.then:", branchSuffix));

    for (auto&& statement : ScopeIfTrue->Expressions)
    {
        statement->ToIR(stack);
    }
    stack.Operation(fmt::format("br label %if{}.end", branchSuffix));
    stack.AdvanceActive(0);
    stack.Label(fmt::format("if{}.else:", branchSuffix));

    for (auto&& statement : ScopeIfFalse->Expressions)
    {
        statement->ToIR(stack);
    }
    stack.Operation(fmt::format("br label %if{}.end", branchSuffix));

    stack.Label(fmt::format("if{}.end:", branchSuffix));
    if (HasElseClauseDefined)
    {
        stack.Operation("unreachable");
    }
    stack.Comment("END RETURN\n");
}

std::vector<Expression*> IfStatementExpression::SubExpressions()
{
    std::vector<Expression*> exprs = { Condition.get() };
    for (auto&& expr : ScopeIfTrue->Expressions)
    {
        exprs.push_back(expr.get());
    }
    for (auto&& expr : ScopeIfFalse->Expressions)
    {
        exprs.push_back(expr.get());
    }
    return exprs;
}

void IfStatementExpression::TypeCheck(Scope* scope)
{
    // TODO: check if return type of inner scopes matches func
    if (Condition->ResolvedType != BUILTIN_BOOL)
    {
        Log::TYPESYS->error("Cannot implicitly convert from '{}' to 'bool'", Condition->ResolvedType->Name);
        Token->Indicate();
        throw type_error("");
    }
}

}
