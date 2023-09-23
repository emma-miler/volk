#include "../../expression/value_indirect.h"

namespace Volk
{
std::string IndirectValueExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "IndirectValueExpression";
    if (ResolvedType != nullptr)
    {
        out += newline + fmt::format("type={}", ResolvedType->Name);
    }
    out += newline + fmt::format("value={}", Value);
    return  out;
}

void IndirectValueExpression::ToIR(ExpressionStack& stack)
{
    // If it is already the most recent value on the stack, we don't need to do anything
    if (Value == stack.ActiveVariable.Name) return;
    stack.AdvanceActive(0);
    std::string variableName = stack.ActiveVariable.Name;
    stack.Comment("START INDIRECT VALUE");
    // Assign the value
    // TODO: figure out the type of Value here
    if (ResolvedVariable == nullptr)
    {
        Log::TYPESYS->error("Unknown variable '{}'", Value);
        Token->Indicate();
        throw type_error("");
    }
    stack.Operation(fmt::format("%{} = load {}, ptr %{}", variableName, ResolvedVariable->Type->LLVMType, Value));
    stack.ActiveVariable.Type = ResolvedVariable->Type->LLVMType;
    stack.Comment("END INDIRECT VALUE");
}

void IndirectValueExpression::ResolveNames(Scope* scope)
{
    ResolvedVariable = scope->FindVariable(Value);
    if (ResolvedVariable == nullptr)
    {
        Log::TYPESYS->error("Unknown variable '{}'", Value);
        Token->Indicate();
        scope->Indicate();
        throw type_error("");
    }
    ResolvedType = ResolvedVariable->Type;
    if (ResolvedVariable->Type == nullptr)
    {
        Log::TYPESYS->error("Resolved variable '{}' does not have a type", ResolvedVariable->Name);
        Token->Indicate();
        scope->Indicate();
        throw type_error("");
    }
}
}
