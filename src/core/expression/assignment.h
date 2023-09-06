#pragma once
#include "../expression_base.h"
#include "value_immediate.h"

#include "../variable.h"
#include "../scope.h"

namespace Volk
{
class AssignmentExpression : public Expression
{
public:
    std::string Name;
    std::unique_ptr<ValueExpression> Value;

    std::shared_ptr<Variable> ResolvedVariable;

public:
    AssignmentExpression(std::string name, std::unique_ptr<ValueExpression> value, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::Assignment, token)
    {
        Name = name;
        Value = std::move(value);
    }
public:
    std::string ToString()
    {
        return fmt::format("AssignmentExpression(name='{}', value={})", Name, Value->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix)
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

    virtual void ToIR(ExpressionStack& stack)
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

    std::vector<Expression*> SubExpressions()
    {
        return std::vector<Expression*>{ Value.get() };
    }

    virtual void ResolveNames(Scope* scope)
    {
        ResolvedVariable = scope->FindVariable(Name);
        if (ResolvedVariable == nullptr)
        {
            Log::TYPESYS->error("Unknown variable '{}'", Name);
            Token->Indicate();
            throw type_error("");
        }
    }

    virtual void TypeCheck(Scope* scope)
    {
        if (ResolvedVariable->Type != Value->ResolvedType)
        {
            Log::TYPESYS->error("Cannot implicitly convert from '{}' to '{}'", Value->ResolvedType->Name, ResolvedVariable->Type->Name);
            Token->Indicate();
            throw type_error("");
        }
    }
};
}
