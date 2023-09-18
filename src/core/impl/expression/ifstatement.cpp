#include "../../expression/ifstatement.h"

namespace Volk
{

std::string IfStatementExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}\t", depthPrefix);
    std::string out = "IfStatementExpression(";
    out += newline + fmt::format("condition={}\t{}", newline, Condition->ToHumanReadableString(depthPrefix + "\t\t"));
    out += newline + fmt::format("scope=[", newline, Condition->ToHumanReadableString(depthPrefix + "\t"));
    for (auto&& statement : InnerScope->Expressions)
    {
        out += newline + fmt::format("\t{}{},", depthPrefix, statement->ToHumanReadableString(depthPrefix + "\t\t"));
    }
    out = out.substr(0, out.size() - 1);
    out += depthPrefix + "]";
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
    stack.AdvanceActive(0);
    stack.Operation(fmt::format("{} = trunc {} to i1", stack.ActiveVariable.GetOnlyName(), conditionVar.Get()));
    std::string branchSuffix = stack.ActiveVariable.Name;
    stack.Operation(fmt::format("br i1 {}, label %if{}.then, label %if{}.end", stack.ActiveVariable.GetOnlyName(), branchSuffix, branchSuffix));
    stack.Label(fmt::format("if{}.then:", branchSuffix));

    for (auto&& statement : InnerScope->Expressions)
    {
        statement->ToIR(stack);
    }
    stack.Operation(fmt::format("br label %if{}.end", branchSuffix));

    stack.Label(fmt::format("if{}.end:", branchSuffix));
    stack.Comment("END RETURN\n");
}

std::vector<Expression*> IfStatementExpression::SubExpressions()
{
    std::vector<Expression*> exprs = { Condition.get() };
    for (auto&& expr : InnerScope->Expressions)
    {
        exprs.push_back(expr.get());
    }
    return exprs;
}

void IfStatementExpression::TypeCheck(Scope* scope)
{
    if (Condition->ResolvedType != BUILTIN_BOOL)
    {
        Log::TYPESYS->error("Cannot implicitly convert from '{}' to 'bool'", Condition->ResolvedType->Name);
        Token->Indicate();
        throw type_error("");
    }
}

}
