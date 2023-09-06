#pragma once
#include "../expression_base.h"
#include "../token/value.h"

namespace Volk
{
class ImmediateValueExpression : public ValueExpression
{
public:
    std::string Value;

public:
    ImmediateValueExpression(std::shared_ptr<ValueToken> token) : ValueExpression(ValueExpressionType::Immediate, OperatorArity::Nullary, token)
    {
        Value = token->Value;
        ResolvedType = token->ValueType;
    }

    std::string ToString()
    {
        return fmt::format("ImmediateValueExpression(value='{}')", Value);
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
        std::string newline = fmt::format("\n{}\t", depthPrefix);
        std::string out = "ImmediateValueExpression(";
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
        stack.AdvanceActive(1);
        // Allocate the variable
        stack.Comment("START IMMEDIATE VALUE");
        stack.Expressions.push_back(fmt::format("%{} = alloca i64", stack.ActiveVariable.Name));
        // Assign the value
        stack.Expressions.push_back(fmt::format("store i64 {}, {}", Value, stack.ActiveVariable.Get()));
        stack.Comment("END IMMEDIATE VALUE\n");
    }
};

}
