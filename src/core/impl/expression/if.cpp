#include "../../expression/if.h"

namespace Volk
{

std::string IfStatementExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "IfStatementExpression";
    out += newline + fmt::format("condition={}", Condition->ToHumanReadableString(depthPrefix + INDENT + INDENT));
    out += newline + "if_true=";
    for (auto&& statement : ScopeIfTrue->Expressions)
    {
        out += newline + fmt::format("{}{},", depthPrefix + INDENT, statement->ToHumanReadableString(depthPrefix + INDENT + INDENT));
    }
    out = out.substr(0, out.size() - 1);
    out += newline + "if_false={}";
    for (auto&& statement : ScopeIfFalse->Expressions)
    {
        out += newline + fmt::format("{}{},", depthPrefix + INDENT, statement->ToHumanReadableString(depthPrefix + INDENT + INDENT));
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
        stack.Operation("%{} = load i1, {}", stack.ActiveVariable.Name, conditionVar.Get());
        conditionVar = stack.ActiveVariable;
    }
    if (stack.ActiveVariable.Type != "i1")
    {
        stack.AdvanceActive(0);
        stack.Operation("{} = trunc {} to i1", stack.ActiveVariable.GetOnlyName(), conditionVar.Get());
    }
    std::string branchSuffix = std::to_string(stack.SpecialCounter++);
	if (HasElseClauseDefined)
	{
		stack.Branch(stack.ActiveVariable, fmt::format("if{}.then", branchSuffix), fmt::format("if{}.else", branchSuffix));
	}
	else
	{
		stack.Branch(stack.ActiveVariable, fmt::format("if{}.then", branchSuffix), fmt::format("if{}.end", branchSuffix));
    }
	stack.Label("if{}.then:", branchSuffix);

    for (auto&& statement : ScopeIfTrue->Expressions)
    {
        statement->ToIR(stack);
    }
    stack.Jump("if{}.end", branchSuffix);
	// I dont really know why LLVM wants this here
	if (HasElseClauseDefined)
	{
		stack.AdvanceActive(0);
		stack.Label("if{}.else:", branchSuffix);

		for (auto&& statement : ScopeIfFalse->Expressions)
		{
			statement->ToIR(stack);
		}
		stack.Jump("if{}.end", branchSuffix);		
	}

    stack.Label("if{}.end:", branchSuffix);
    if (HasElseClauseDefined)
    {
        stack.Operation("unreachable");
    }
    stack.Comment("END RETURN\n");
}

std::vector<std::shared_ptr<Expression>> IfStatementExpression::SubExpressions()
{
    std::vector<std::shared_ptr<Expression>> exprs = { Condition };
    for (auto&& expr : ScopeIfTrue->Expressions)
    {
        exprs.push_back(expr);
    }
    for (auto&& expr : ScopeIfFalse->Expressions)
    {
        exprs.push_back(expr);
    }
    return exprs;
}

void IfStatementExpression::ResolveNames(Scope* scope)
{
    Condition->ResolveNames(scope);
    for (auto&& expr : ScopeIfTrue->Expressions)
    {
        expr->ResolveNames(scope);
    }
    for (auto&& expr : ScopeIfFalse->Expressions)
    {
        expr->ResolveNames(scope);
    }
}

void IfStatementExpression::TypeCheck(Scope* scope)
{
    Condition->TypeCheck(scope);
    for (auto&& expr : ScopeIfTrue->Expressions)
    {
        expr->TypeCheck(scope);
    }
    for (auto&& expr : ScopeIfFalse->Expressions)
    {
        expr->TypeCheck(scope);
    }
    // TODO: check if return type of inner scopes matches func
    if (Condition->ResolvedType != BUILTIN_BOOL)
    {
        Log::TYPESYS->error("Cannot implicitly convert from '{}' to 'bool'", Condition->ResolvedType->Name);
        Token->Indicate();
        throw type_error("");
    }
}

}
