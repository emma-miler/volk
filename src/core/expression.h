#pragma once

#include <string>
#include <memory>
#include "spdlog/spdlog.h"

#include "operator.h"

namespace Volk
{

class VKExpression;

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

    virtual std::string ToIR()
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

    virtual std::string ToIR()
    {
        return fmt::format("%{} = alloca i32, align 4", Name);
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

    virtual std::string ToIR()
    {
        throw std::runtime_error("Pure ValueExpession cannot be converted directly to LLVM IR");
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

    virtual std::string ToIR()
    {
        // TODO: fix this, unravel calculation stack
        // return fmt::format("store i32 {}, ptr %{}, align 4", Value->Value, Name);
        return "ERR";
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

    virtual std::string ToIR()
    {
        // TODO: fix this, unravel calculation stack
        // return fmt::format("%ret = load i32, ptr %{}, align 4\n\tret i32 %ret", Value->Value);
        return "ERR";
    }
};


}
