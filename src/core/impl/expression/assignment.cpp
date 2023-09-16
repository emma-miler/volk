#include "../../expression/assignment.h"

namespace Volk
{
std::string AssignmentExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}\t", depthPrefix);
    std::string out = "AssignmentExpression(";
    if (ResolvedVariable != nullptr)
    {
        out += newline + fmt::format("type='{}'", ResolvedVariable->Type->Name);
    }
    out += newline + fmt::format("name='{}'", Name);
    out += newline + fmt::format("value='{}'", Value->ToHumanReadableString(depthPrefix + "\t"));
    out += "\n" + depthPrefix + ")";
    return  out;
}

void AssignmentExpression::ToIR(ExpressionStack& stack)
{
    if (Value->ValueExpressionType == ValueExpressionType::Immediate)
    {
        ImmediateValueExpression* value = static_cast<ImmediateValueExpression*>(Value.get());
        stack.Comment("START ASSIGNMENT");
        stack.Expressions.push_back(fmt::format("store {} {}, ptr %{}", value->ResolvedType->LLVMType, value->Value, Name));
    }
    else
    {
        stack.Comment("START ASSIGNMENT VALUE");
        Value->ToIR(stack);
        IRVariableDescriptor value = stack.ActiveVariable;
        stack.Comment("START ASSIGNMENT");
        stack.Expressions.push_back(fmt::format("store {}, ptr %{}", value.Get(), Name));
    }
    stack.Comment("END ASSIGNMENT\n");
}

std::vector<Expression*> AssignmentExpression::SubExpressions()
{
    return std::vector<Expression*>{ Value.get() };
}

void AssignmentExpression::ResolveNames(Scope* scope)
{
    ResolvedVariable = scope->FindVariable(Name);
    if (ResolvedVariable == nullptr)
    {
        Log::TYPESYS->error("Unknown variable '{}'", Name);
        Token->Indicate();
        throw type_error("");
    }
}
void AssignmentExpression::TypeCheck(Scope* scope)
{
    if (ResolvedVariable->Type != Value->ResolvedType)
    {
        Log::TYPESYS->error("Cannot implicitly convert from '{}' to '{}'", Value->ResolvedType->Name, ResolvedVariable->Type->Name);
        Token->Indicate();
        throw type_error("");
    }
}
}
