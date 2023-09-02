
#pragma once

#include <string>
#include <memory>
#include "spdlog/spdlog.h"

#include "irvariabledescriptor.h"
#include "../util/options.h"
#include "operator.h"
#include "type.h"
#include "token.h"

namespace Volk
{

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
    Immediate,
    Indirect,
    Unary,
    Binary,
    FunctionCall,
    StringConstant,
};

enum class OperatorArity
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
        ActiveVariable.IsConstant = 0;
    }

    void Comment(std::string comment)
    {
        if (!FF_LLVM_VERBOSE) return;
        Expressions.push_back("; " + comment);
    }
};

class Expression
{
public:
    ExpressionType Type;
    std::shared_ptr<Volk::Token> Token;


public:
    Expression(ExpressionType type, std::shared_ptr<Volk::Token> token)
    {
        Type = type;
        Token = token;
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

    virtual std::vector<Expression*> SubExpressions()
    {
        return std::vector<Expression*>{};
    }

};

/// ==========
/// Value Expressions
/// ==========

class ValueExpression : public Expression
{
public:
    Volk::ValueExpressionType ValueExpressionType;
    OperatorArity Arity;
    std::shared_ptr<Volk::Type> VariableType;

public:
    ValueExpression(Volk::ValueExpressionType valueType,  OperatorArity arity, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::Value, token)
    {
        ValueExpressionType = valueType;
        Arity = arity;
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

}
