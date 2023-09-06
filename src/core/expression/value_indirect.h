#pragma once
#include "../expression_base.h"

namespace Volk
{
class IndirectValueExpression : public ValueExpression
{
public:
    std::string Value;

    std::shared_ptr<Volk::Variable> ResolvedVariable;

public:
    IndirectValueExpression(std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::Indirect, OperatorArity::Nullary, token)
    {
        Value = token->Value;
    }

    std::string ToString()
    {
        return fmt::format("IndirectValueExpression(value='{}')", Value);
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
        std::string newline = fmt::format("\n{}\t", depthPrefix);
        std::string out = "Indirect(";
        if (ResolvedType != nullptr)
        {
            out += newline + fmt::format("type='{}'", ResolvedType->Name);
        }
        out += newline + fmt::format("value='{}'", Value);
        out += "\n" + depthPrefix + ")";
        return  out;
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        // If it is already the most recent value on the stack, we don't need to do anything
        if (Value == stack.ActiveVariable.Name) return;
        stack.AdvanceActive(0);
        std::string variableName = stack.ActiveVariable.Name;
        stack.Comment("START INDIRECT VALUE");
        // Assign the value
        // TODO: figure out the type of Value here
        stack.Expressions.push_back(fmt::format("%{} = load {}, ptr %{}", variableName, ResolvedVariable->Type->LLVMType, Value));
        stack.ActiveVariable.Type = ResolvedVariable->Type->LLVMType;
        stack.Comment("END INDIRECT VALUE");
    }

    virtual void ResolveNames(Scope* scope)
    {
        ResolvedVariable = scope->FindVariable(Value);
        if (ResolvedVariable == nullptr)
        {
            Log::TYPESYS->error("Unknown variable '{}'", Value);
            Token->Indicate();
            throw type_error("");
        }
        ResolvedType = ResolvedVariable->Type;
    }
};
}
