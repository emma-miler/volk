#pragma once

#include <string>
#include <memory>
#include "spdlog/spdlog.h"

#include "irvariabledescriptor.h"

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
    FunctionCall,
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
    IRVariableDescriptor ActiveVariable;

public:
    ExpressionStack() : ActiveVariable("", 0)
    {
        NameCounter = 1;
    }

    // Advances the stack's active temporary variable by 1
    void AdvanceActive(bool isPointer)
    {
        ActiveVariable.Name = std::to_string(NameCounter++);
        ActiveVariable.IsPointer = isPointer;
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
        return fmt::format("ImmediateValueExpression(\n{}\tvalue='{}'\n{})", depthPrefix, Value, depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        stack.AdvanceActive(1);
        // Allocate the variable
        stack.Expressions.push_back("; START IMMEDIATE VALUE");
        stack.Expressions.push_back(fmt::format("%{} = alloca i32, align 4", stack.ActiveVariable.Name));
        // Assign the value
        stack.Expressions.push_back(fmt::format("store i32 {}, ptr %{}, align 4", Value, stack.ActiveVariable.Name));
        stack.Expressions.push_back("; END IMMEDIATE VALUE\n");

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
        return fmt::format("IndirectValueExpression(\n{}\tvalue='{}'\n{})", depthPrefix, Value, depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        stack.AdvanceActive(0);
        std::string variableName = stack.ActiveVariable.Name;
        stack.Expressions.push_back("; START INDIRECT VALUE");
        // Assign the value
        stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", variableName, Value));
        stack.Expressions.push_back("; END INDIRECT VALUE\n");
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
         return fmt::format("UnaryValueExpression(\n{}\top={}, \n{}\tvalue={}\n{})", depthPrefix, OperatorTypeNames[Operator], depthPrefix, Value->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        Value->ToIR(stack);
        std::string valueVariableName = stack.ActiveVariable.Name;
        // Perform the operator
        stack.Expressions.push_back("; START UNARY OPERATOR");
        if (stack.ActiveVariable.IsPointer)
        {
            stack.AdvanceActive(0);
            stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", stack.ActiveVariable.Name, valueVariableName));
            valueVariableName = stack.ActiveVariable.Name;
        }
        stack.AdvanceActive(0);
        stack.Expressions.push_back(fmt::format("%{} = {} nsw i32 0, %{}", stack.ActiveVariable.Name, Operator == OperatorType::OperatorMinus ? "sub" : "add", valueVariableName));
        stack.Expressions.push_back("; END UNARY OPERATOR\n");
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
        return fmt::format("BinaryValueExpression(\n{}\top={}, \n{}\tleft={}, \n{}\tright={}\n{})",
                           depthPrefix, OperatorTypeNames[Operator], depthPrefix,  Left->ToHumanReadableString(depthPrefix + "\t"), depthPrefix, Right->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        Left->ToIR(stack);
        IRVariableDescriptor left = stack.ActiveVariable;

        Right->ToIR(stack);
        IRVariableDescriptor right = stack.ActiveVariable;

        // LLVM does not allow you to operate on an immediate i32 with a ptr value or vice versa,
        // and requires that the type of both operands is the same,
        // so we must load the left and/or right side into a temp variable
        if (left.IsPointer || right.IsPointer)
        {
            stack.Expressions.push_back("; START BINARY OPERATOR PRELOAD");
            if (left.IsPointer)
            {
                stack.AdvanceActive(0);
                stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", stack.ActiveVariable.Name, left.Name));
                left.Name = stack.ActiveVariable.Name;
            }
            if (right.IsPointer)
            {
                stack.AdvanceActive(0);
                stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", stack.ActiveVariable.Name, right.Name));
                right.Name = stack.ActiveVariable.Name;
            }
            stack.Expressions.push_back("; END BINARY OPERATOR PRELOAD\n");
        }

        // Perform the operator
        stack.AdvanceActive(0);
        stack.Expressions.push_back("; START BINARY OPERATOR");
        stack.Expressions.push_back(fmt::format("%{} = {} i32 %{}, %{}", stack.ActiveVariable.Name, OperatorInstructionLookup[Operator], left.Name, right.Name));
        stack.Expressions.push_back("; END BINARY OPERATOR\n");
    }
};

// Holds variable names
class FunctionCallValueExpression : public ValueExpression
{
public:
    std::string FunctionName;
    std::vector<std::unique_ptr<ValueExpression>> Arguments;

public:
    FunctionCallValueExpression(std::string functionName) : ValueExpression(ValueExpressionType::FunctionCall)
    {
        FunctionName = functionName;
    }

    std::string ToString()
    {
        return fmt::format("FunctionCallValueExpression(\nname='{}')", FunctionName);
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
        std::string res = fmt::format("FunctionCallValueExpression(\n{}\tname='{}'\n{}\targs=[", depthPrefix, FunctionName, depthPrefix);

        for (auto&& arg : Arguments)
        {
            res += fmt::format("\n{}\t\t{},", depthPrefix, arg->ToHumanReadableString(depthPrefix + "\t\t"));
        }

        res += fmt::format("\n{}\t]\n{})", depthPrefix, depthPrefix);
        return res;
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        std::vector<IRVariableDescriptor> argumentNames;
        // Calculate all of our arguments
        stack.Expressions.push_back("; START FUNCTION CALL ARGUMENTS");
        for (auto&& arg : Arguments)
        {
            arg->ToIR(stack);
            argumentNames.push_back(stack.ActiveVariable);
        }
        stack.Expressions.push_back("; END FUNCTION CALL ARGUMENTS");
        stack.Expressions.push_back("; START FUNCTION CALL");
        stack.AdvanceActive(0);
        std::string ir = fmt::format("%{} = call noundef i32 @{}(", stack.ActiveVariable.Name, FunctionName);
        for (auto&& arg : argumentNames)
        {
            ir += fmt::format("ptr %{}, ", arg.Name);
        }
        ir = ir.substr(0, ir.length() - 2);
        ir += ")";
        stack.Expressions.push_back(ir);
        stack.Expressions.push_back("; END FUNCTION CALL\n");
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
         return fmt::format("AssignmentExpression(\n{}\tname='{}', \n{}\tvalue={}\n{})", depthPrefix, Name, depthPrefix, Value->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
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
            std::string valueName = stack.ActiveVariable.Name;
            stack.Expressions.push_back("; START ASSIGNMENT");
            stack.Expressions.push_back(fmt::format("store {} %{}, ptr %{}, align 4", stack.ActiveVariable.IsPointer ? "ptr" : "i32", valueName, Name));
        }
        stack.Expressions.push_back("; END ASSIGNMENT\n");
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

    std::string ToHumanReadableString(std::string depthPrefix)
    {
         return fmt::format("ReturnExpression(\n{}\tvalue={}\n{})", depthPrefix, Value->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        Value->ToIR(stack);
        stack.Expressions.push_back("; START RETURN");
        if (stack.ActiveVariable.IsPointer)
        {
            stack.Expressions.push_back(fmt::format("%{} = load i32, ptr %{}, align 4", stack.ActiveVariable.Name, stack.ActiveVariable.Name));
            stack.Expressions.push_back(fmt::format("ret i32 %{}", stack.ActiveVariable.Name));
        }
        else
        {
            stack.Expressions.push_back(fmt::format("ret i32 %{}", stack.ActiveVariable.Name));
        }
        stack.Expressions.push_back("; END RETURN\n");
    }
};


}
