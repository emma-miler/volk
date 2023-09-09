#pragma once
#include "../expression_base.h"

namespace Volk
{
class BinaryValueExpression : public ValueExpression
{
public:
    std::unique_ptr<ValueExpression> Left;
    std::unique_ptr<ValueExpression> Right;
    OperatorType Operator;

public:
    BinaryValueExpression(OperatorType op, std::unique_ptr<ValueExpression> left, std::unique_ptr<ValueExpression> right, std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::Binary, OperatorArity::Binary, token)
    {
        Left = std::move(left);
        Right = std::move(right);
        Operator = op;
    }

    std::string ToString()
    {
        return fmt::format("BinaryValueExpression(op={}, left={}, right={})",
                           OperatorTypeNames[Operator], Left->ToString(), Right->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
        std::string newline = fmt::format("\n{}\t", depthPrefix);
        std::string out = "BinaryOperatorValueExpression(";
        if (ResolvedType != nullptr)
        {
            out += newline + fmt::format("type='{}'", ResolvedType->Name);
        }
        out += newline + fmt::format("op='{}'", OperatorTypeNames[Operator]);
        out += newline + fmt::format("left='{}'", Left->ToHumanReadableString(depthPrefix + "\t"));
        out += newline + fmt::format("right='{}'", Right->ToHumanReadableString(depthPrefix + "\t"));
        out += "\n" + depthPrefix + ")";
        return  out;
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        Left->ToIR(stack);
        IRVariableDescriptor left = stack.ActiveVariable;

        Right->ToIR(stack);
        IRVariableDescriptor right = stack.ActiveVariable;

        // LLVM does not allow you to operate on an immediate i64 with a ptr value or vice versa,
        // and requires that the type of both operands is the same,
        // so we must load the left and/or right side into a temp variable
        if (left.IsPointer || right.IsPointer)
        {
            stack.Comment("START BINARY OPERATOR PRELOAD");
            if (left.IsPointer)
            {
                stack.AdvanceActive(0);
                stack.Expressions.push_back(fmt::format("%{} = load i64, {}", stack.ActiveVariable.Name, left.Get()));
                left.Name = stack.ActiveVariable.Name;
                left.IsPointer = 0;
            }
            if (right.IsPointer)
            {
                stack.AdvanceActive(0);
                stack.Expressions.push_back(fmt::format("%{} = load i64, {}", stack.ActiveVariable.Name, right.Get()));
                right.Name = stack.ActiveVariable.Name;
                right.IsPointer = 0;
            }
            stack.Comment("END BINARY OPERATOR PRELOAD\n");
        }

        // Perform the operator
        stack.AdvanceActive(0);
        stack.Comment("START BINARY OPERATOR");
        stack.Expressions.push_back(fmt::format("%{} = {}{} {}, {}", stack.ActiveVariable.Name, Left->ResolvedType == BUILTIN_FLOAT || Left->ResolvedType == BUILTIN_DOUBLE ? "f" : "", OperatorInstructionLookup[Operator], left.Get(), right.GetOnlyName()));
        stack.Comment("END BINARY OPERATOR\n");
    }

    std::vector<Expression*> SubExpressions()
    {
        return std::vector<Expression*>{ Left.get(), Right.get() };
    }

    virtual void TypeCheck(Scope* scope)
    {
        if (Left->ResolvedType != Right->ResolvedType)
        {
            Log::TYPESYS->error("No valid operator '{}' between types '{}' and '{}'", OperatorTypeNames[Operator], Left->ResolvedType->Name, Right->ResolvedType->Name);
            Token->Indicate();
            throw type_error("");
        }
        // TODO: actually do this properly
        ResolvedType = Left->ResolvedType;
    }

};
}
