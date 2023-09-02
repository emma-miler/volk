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
        return fmt::format("BinaryValueExpression(\n{}\top={}, \n{}\tleft={}, \n{}\tright={}\n{})",
                           depthPrefix, OperatorTypeNames[Operator], depthPrefix,  Left->ToHumanReadableString(depthPrefix + "\t"), depthPrefix, Right->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
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
        stack.Expressions.push_back(fmt::format("%{} = {} {}, {}", stack.ActiveVariable.Name, OperatorInstructionLookup[Operator], left.Get(), right.GetOnlyName()));
        stack.Comment("END BINARY OPERATOR\n");
    }

    std::vector<Expression*> SubExpressions()
    {
        return std::vector<Expression*>{ Left.get(), Right.get() };
    }
};
}
