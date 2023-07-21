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

class ValueExpression : public Expression
{
public:
    std::string Value;

public:
    ValueExpression(std::string value) : Expression(ExpressionType::Value)
    {
        Value = value;
    }

public:
    std::string ToString()
    {
        return fmt::format("ValueExpression(value='{}')", Value);
    }

    virtual std::string ToIR()
    {
        throw std::runtime_error("Pure ValueExpession cannot be converted directly to LLVM IR");
    }
};

class BinaryOperatorExpression : public Expression
{
public:
    std::unique_ptr<Expression> Left;
    std::unique_ptr<Expression> Right;
    OperatorType Operator;

public:
    BinaryOperatorExpression(OperatorType op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) : Expression(ExpressionType::BinaryOperator)
    {
        Left = std::move(left);
        Right = std::move(right);
        Operator = op;
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
        return fmt::format("AssignmentExpression(name='{}', value='{}')", Name, Value->ToString());
    }

    virtual std::string ToIR()
    {
        return fmt::format("store i32 {}, ptr %{}, align 4", Value->Value, Name);
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
        return fmt::format("%ret = load i32, ptr %{}, align 4\n\tret i32 %ret", Value->Value);
    }
};


}
