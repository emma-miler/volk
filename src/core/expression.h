#pragma once

#include <string>
#include <memory>
#include "spdlog/spdlog.h"

#include "operator.h"

namespace Volk
{

class Expression;

#include "../parsing/parser.h"

enum class ExpressionType
{
    Declaration,
    Assignment,
    Call,
    Value,
    BinaryOperator,
    ReturnExpr,
};


enum class ValueExpressionType
{
    Nullary,
    Unary,
    Binary,
};

static std::map<ExpressionType, std::string> ExpressionTypeNames =
{
    {ExpressionType::Declaration, "Declaration"},
    {ExpressionType::Assignment, "Assignment"},
    {ExpressionType::Call, "Call"},
    {ExpressionType::Value, "Value"},
    {ExpressionType::BinaryOperator, "BinaryOperator"},
    {ExpressionType::ReturnExpr, "ReturnExpr"}
};

class ExpressionStack
{
public:
    std::vector<std::string> Expressions;
    int NameCounter;
    std::string ActiveVariable;
    int IsActivePtr;

public:
    ExpressionStack()
    {
        NameCounter = 0;
        ActiveVariable = "";
        IsActivePtr = 0;
    }
};

class Expression
{
public:
    ExpressionType Type;

public:
    Expression(ExpressionType type)
    {
        Type = type;
    }

public:
    virtual std::string ToString()
    {
        throw std::runtime_error("Pure virtual call to Expression::ToString()");
    }

    // Output is the variable name
    virtual void ToIR(ExpressionStack& stack)
    {
        throw std::runtime_error("Pure virtual call to Expression::ToIR()");
    }

    virtual std::string ToHumanReadableString(std::string depthPrefix)
    {
        return ToString();
    }

};

class DeclarationExpression : public Expression
{
public:
    std::string Typename;
    std::string Name;

public:
    DeclarationExpression(std::string type, std::string name) : Expression(ExpressionType::Declaration)
    {
        Typename = type;
        Name = name;
    }
public:
    std::string ToString()
    {
        return fmt::format("DeclarationExpression(type='{}', name='{}')", Typename, Name);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        stack.Expressions.push_back("; START DECLARATION");
        stack.Expressions.push_back(fmt::format("%{} = alloca i32, align 4", Name));
        stack.Expressions.push_back("; END DECLARATION\n");
    }
};

/// ==========
/// Value Expressions
/// ==========

class ValueExpression : public Expression
{
public:
    ValueExpressionType OperatoryArity;

public:
    ValueExpression(ValueExpressionType arity) : Expression(ExpressionType::Value)
    {
        OperatoryArity = arity;
    }

public:
    virtual std::string ToString()
    {
        // TODO: make this automatically cast the this pointer to the correct type
        // by using OperatorType, and call the ToString of that type instead
        return fmt::format("Pure virtual call to ValueExpression::ToString()");
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        throw std::runtime_error("Pure ValueExpression cannot be converted directly to LLVM IR");
    }
};

// Holds immediate values
class ImmediateValueExpression : public ValueExpression
{
public:
    std::string Value;

public:
    ImmediateValueExpression(std::string value) : ValueExpression(ValueExpressionType::Nullary)
    {
        Value = value;
    }

    std::string ToString()
    {
        return fmt::format("ImmediateValueExpression(value='{}')", Value);
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
        return fmt::format("ImmediateValueExpression(\n{}value='{}'\n{})", depthPrefix, Value, depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        int variableName = stack.NameCounter++;
        // Allocate the variable
        stack.Expressions.push_back("; START IMMEDIATE VALUE");
        stack.Expressions.push_back(fmt::format("%{} = alloca i32, align 4", variableName));
        // Assign the value
        stack.Expressions.push_back(fmt::format("store i32 {}, ptr %{}, align 4", Value, variableName));
        stack.Expressions.push_back("; END IMMEDIATE VALUE\n");
        stack.ActiveVariable = std::to_string(variableName);
        stack.IsActivePtr = 1;
    }
};

// Holds variable names
class IndirectValueExpression : public ValueExpression
{
public:
    std::string Value;

public:
    IndirectValueExpression(std::string value) : ValueExpression(ValueExpressionType::Nullary)
    {
        Value = value;
    }

    std::string ToString()
    {
        return fmt::format("IndirectValueExpression(value='{}')", Value);
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
        return fmt::format("IndirectValueExpression(\n{}value='{}'\n{})", depthPrefix, Value, depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        int variableName = stack.NameCounter++;
        stack.Expressions.push_back("; START INDIRECT VALUE");
        // Assign the value
        stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", variableName, Value));
        stack.Expressions.push_back("; END INDIRECT VALUE\n");
        stack.ActiveVariable = std::to_string(variableName);
        stack.IsActivePtr = 0;
    }
};

class UnaryValueExpression : public ValueExpression
{
public:
    std::unique_ptr<Expression> Value;
    OperatorType Operator;

public:
    UnaryValueExpression(OperatorType op, std::unique_ptr<Expression> value) : ValueExpression(ValueExpressionType::Unary)
    {
        Value = std::move(value);
        Operator = op;
    }

    std::string ToString()
    {
        return fmt::format("UnaryValueExpression(op={}, value={})", OperatorTypeNames[Operator], Value->ToString());
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
         return fmt::format("UnaryValueExpression(\n{}op={}, \n{}value={}\n{})", depthPrefix, OperatorTypeNames[Operator], depthPrefix, Value->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        Value->ToIR(stack);
        std::string valueVariableName = stack.ActiveVariable;
        // Perform the operator
        stack.Expressions.push_back("; START UNARY OPERATOR");
        if (stack.IsActivePtr)
        {
            int variableName = stack.NameCounter++;
            stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", variableName, valueVariableName));
            valueVariableName = std::to_string(variableName);
        }
        int variableName = stack.NameCounter++;
        stack.Expressions.push_back(fmt::format("%{} = {} nsw i32 0, %{}", variableName, Operator == OperatorType::OperatorMinus ? "sub" : "add", valueVariableName));
        stack.Expressions.push_back("; END UNARY OPERATOR\n");
        stack.ActiveVariable = std::to_string(variableName);
        stack.IsActivePtr = 0;
    }
};

class BinaryValueExpression : public ValueExpression
{
public:
    std::unique_ptr<Expression> Left;
    std::unique_ptr<Expression> Right;
    OperatorType Operator;

public:
    BinaryValueExpression(OperatorType op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) : ValueExpression(ValueExpressionType::Binary)
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
        return fmt::format("BinaryValueExpression(\n{}op={}, \n{}left={}, \n{}right={}\n{})",
                           depthPrefix, OperatorTypeNames[Operator], depthPrefix,  Left->ToHumanReadableString(depthPrefix + "\t"), depthPrefix, Right->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        Left->ToIR(stack);
        std::string leftName = stack.ActiveVariable;
        int isLeftPtr = stack.IsActivePtr;

        Right->ToIR(stack);
        std::string rightName = stack.ActiveVariable;
        int isRightPtr = stack.IsActivePtr;

        // LLVM does not allow you to operate on an immediate i32 with a ptr value or vice versa,
        // and requires that the type of both operands is the same,
        // so we must load the left and/or right side into a temp variable
        if (isLeftPtr || isRightPtr)
        {
            stack.Expressions.push_back("; START BINARY OPERATOR PRELOAD");
            if (isLeftPtr)
            {
                int variableName = stack.NameCounter++;
                stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", variableName, leftName));
                leftName = std::to_string(variableName);
            }
            if (isRightPtr)
            {
                int variableName = stack.NameCounter++;
                stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", variableName, rightName));
                rightName = std::to_string(variableName);
            }
            stack.Expressions.push_back("; END BINARY OPERATOR PRELOAD\n");
        }


        int variableName = stack.NameCounter++;
        // Perform the operator
        stack.Expressions.push_back("; START BINARY OPERATOR");
        stack.Expressions.push_back(fmt::format("%{} = {} i32 %{}, %{}", variableName, OperatorInstructionLookup[Operator], leftName, rightName));
        stack.Expressions.push_back("; END BINARY OPERATOR\n");
        stack.ActiveVariable = std::to_string(variableName);
        stack.IsActivePtr = 0;
    }
};


class AssignmentExpression : public Expression
{
public:
    std::string Name;
    std::unique_ptr<ValueExpression> Value;

public:
    AssignmentExpression(std::string name, std::unique_ptr<ValueExpression> value) : Expression(ExpressionType::Assignment)
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
         return fmt::format("AssignmentExpression(\n{}name='{}', \n{}value={}\n{})", depthPrefix, Name, depthPrefix, Value->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {

        if (Value->OperatoryArity == ValueExpressionType::Nullary)
        {
            ImmediateValueExpression value = *static_cast<ImmediateValueExpression*>(Value.get());
            stack.Expressions.push_back("; START ASSIGNMENT");
            stack.Expressions.push_back(fmt::format("store i32 {}, ptr %{}, align 4", value.Value, Name));
        }
        else
        {
            Value->ToIR(stack);
            std::string valueName = stack.ActiveVariable;
            stack.Expressions.push_back("; START ASSIGNMENT");
            stack.Expressions.push_back(fmt::format("store {} %{}, ptr %{}, align 4", stack.IsActivePtr ? "ptr" : "i32", valueName, Name));
        }
        stack.Expressions.push_back("; END ASSIGNMENT\n");
        stack.ActiveVariable = "NULL";
    }
};

class ReturnExpression : public Expression
{
public:
    std::unique_ptr<ValueExpression> Value;

public:
    ReturnExpression(std::unique_ptr<ValueExpression> value) : Expression(ExpressionType::ReturnExpr)
    {
        Value = std::move(value);
    }

  public:
    std::string ToString()
    {
        return fmt::format("ReturnExpression(value='{}')", Value->ToString());
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        Value->ToIR(stack);
        int variableName = stack.NameCounter++;
        stack.Expressions.push_back("; START RETURN");
        if (stack.IsActivePtr)
        {
            stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", variableName, stack.ActiveVariable));
            stack.Expressions.push_back(fmt::format("ret i32 %{}", variableName));
        }
        else
        {
            stack.Expressions.push_back(fmt::format("ret i32 %{}", stack.ActiveVariable));
        }
        stack.Expressions.push_back("; END RETURN\n");
    }
};


}
