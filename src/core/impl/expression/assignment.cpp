#include "../../expression/assignment.h"
#include "../../expression/value_valuecast.h"
#include "../../valuecast_rules.h"

namespace Volk
{
std::string AssignmentExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "AssignmentExpression";
    if (ResolvedVariable != nullptr)
    {
        out += newline + fmt::format("type={}", ResolvedVariable->Type->Name);
    }
    out += newline + fmt::format("name={}", Name);
    out += newline + fmt::format("value={}", Value->ToHumanReadableString(depthPrefix + INDENT));
    return  out;
}

void AssignmentExpression::ToIR(ExpressionStack& stack)
{
    if (Value->ValueExpressionType == ValueExpressionType::Immediate)
    {
        ImmediateValueExpression* value = static_cast<ImmediateValueExpression*>(Value.get());
        stack.Comment("START ASSIGNMENT");
        stack.Operation("store {} {}, ptr %{}", value->ResolvedType->LLVMType, value->Value, Name);
    }
    else
    {
        stack.Comment("START ASSIGNMENT VALUE");
        Value->ToIR(stack);
        IRVariableDescriptor value = stack.ActiveVariable;
        stack.Comment("START ASSIGNMENT");
        stack.Operation("store {}, ptr %{}", value.Get(), Name);
    }
    stack.Comment("END ASSIGNMENT\n");
}

std::vector<std::shared_ptr<Expression>> AssignmentExpression::SubExpressions()
{
    return std::vector<std::shared_ptr<Expression>>{ Value };
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
    Value->ResolveNames(scope);
}
void AssignmentExpression::TypeCheck(Scope* scope)
{
    Value->TypeCheck(scope);
    if (ResolvedVariable->Type != Value->ResolvedType)
    {
		auto rule = Value->ResolvedType->ImplicitConverters.find(ResolvedVariable->Type);
		if (rule == Value->ResolvedType->ImplicitConverters.end())
		{
			Log::TYPESYS->error("Cannot implicitly convert from '{}' to '{}'", Value->ResolvedType->Name, ResolvedVariable->Type->Name);
			Token->Indicate();
			throw type_error("");
		}
		else
		{
			Value = std::make_unique<ValueCastExpression>(std::move(Value), ResolvedVariable->Type, rule->second);
		}
    }
}
}
