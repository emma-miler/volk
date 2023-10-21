#include "../../expression/for.h"

namespace Volk
{

std::string ForExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "ForExpression";
    out += newline + fmt::format("initialize={}", Initializer->ToHumanReadableString(depthPrefix + INDENT + INDENT));
    out += newline + fmt::format("condition={}", Condition->ToHumanReadableString(depthPrefix + INDENT + INDENT));
    out += newline + fmt::format("increment={}", Increment->ToHumanReadableString(depthPrefix + INDENT + INDENT));
    out += newline + "scope=";
    for (auto&& statement : InnerScope->Expressions)
    {
        out += newline + fmt::format("{}{},", depthPrefix, statement->ToHumanReadableString(depthPrefix + INDENT + INDENT));
    }
    out = out.substr(0, out.size() - 1);
    return  out;
}

void ForExpression::ToIR(ExpressionStack& stack)
{
    // TODO
    // in its current state, this will quickly run out of stack memory
    // since we both reallocate the iterator and the scope contents every iteration
    // we need to do cleanup of the scope contents, and not reallocate the iterator
    std::string branchSuffix = std::to_string(stack.SpecialCounter++);
    // Initializer
    stack.Comment("START FOR INITIALIZER");
    stack.Operation("%{} = alloca {}", Initializer->Name, Initializer->Value->ResolvedType->LLVMType);
    Initializer->ToIR(stack);
    stack.Comment("END FOR INITIALIZER");

    // Condition
    stack.Comment("START FOR CONDITION");
    stack.Jump("for{}.condition", branchSuffix);
    stack.Label("for{}.condition:", branchSuffix);
    Condition->ToIR(stack);
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

    stack.Branch(stack.ActiveVariable, fmt::format("for{}.body", branchSuffix), fmt::format("for{}.end", branchSuffix));
	stack.LastJumpPoint = fmt::format("for{}.body", branchSuffix);
    stack.Label("for{}.body:", branchSuffix);

    // Inner scope
    for (auto&& statement : InnerScope->Expressions)
    {
        statement->ToIR(stack);
    }
    // Increment
    Increment->ToIR(stack);

    // Branch back to condition
    stack.Jump("for{}.condition", branchSuffix);

    // End
    stack.Label("for{}.end:", branchSuffix);

    stack.Comment("END FOR\n");
}

std::vector<std::shared_ptr<Expression>> ForExpression::SubExpressions()
{
    return std::vector<std::shared_ptr<Expression>>{};
}

void ForExpression::ResolveNames(Scope* scope)
{
    Initializer->ResolveNames(InnerScope.get());
    for (auto&& expr : Initializer->SubExpressions())
    {
        expr->ResolveNames(InnerScope.get());
    }
    Condition->ResolveNames(InnerScope.get());
    for (auto&& expr : Condition->SubExpressions())
    {
        expr->ResolveNames(InnerScope.get());
    }
    Increment->ResolveNames(InnerScope.get());
    for (auto&& expr : Increment->SubExpressions())
    {
        expr->ResolveNames(InnerScope.get());
    }

    for (auto&& expr : InnerScope->Expressions)
    {
        expr->ResolveNames(InnerScope.get());
    }
}

void ForExpression::TypeCheck(Scope* scope)
{
    for (auto&& expr : Initializer->SubExpressions())
    {
        expr->TypeCheck(InnerScope.get());
    }
    Initializer->TypeCheck(InnerScope.get());

    for (auto&& expr : Condition->SubExpressions())
    {
        expr->TypeCheck(InnerScope.get());
    }
    Condition->TypeCheck(InnerScope.get());

    for (auto&& expr : Increment->SubExpressions())
    {
        expr->TypeCheck(InnerScope.get());
    }
    Increment->TypeCheck(InnerScope.get());


    for (auto&& expr : InnerScope->Expressions)
    {
        expr->TypeCheck(InnerScope.get());
    }
}

}
