#pragma once

#include <string>
#include <memory>
#include <exception>

namespace Volk
{
    class Expression;
    class ExpressionStack;
}

#include "irvariabledescriptor.h"
#include "../util/options.h"
#include "type.h"
#include "token.h"
#include "scope.h"

namespace Volk
{

enum class ExpressionType
{
	Comment,
    Declaration,
    FunctionDeclaration,
    Assignment,
    Call,
    Value,
    BinaryOperator,
    ReturnExpr,
    If,
    For,
};


enum class ValueExpressionType
{
    Immediate,
    Indirect,
    Unary,
    Binary,
    FunctionCall,
    StringConstant,
	ValueCast,
};

enum class OperatorArity
{
    Nullary,
    Unary,
    Binary,
};

static std::map<ExpressionType, std::string> ExpressionTypeNames =
{
	{ExpressionType::Comment, "Comment"},
    {ExpressionType::Declaration, "Declaration"},
    {ExpressionType::FunctionDeclaration, "FunctionDeclaration"},
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
	int SpecialCounter;
	std::string LastJumpPoint;

public:
    ExpressionStack() : ActiveVariable("", "", 0, IRVarType::Variable)
    {
        NameCounter = 0;
		SpecialCounter = 0;
		LastJumpPoint = "entry";
    }

    // Advances the stack's active temporary variable by 1
    void AdvanceActive(bool isPointer)
    {
        ActiveVariable.Name = std::to_string(NameCounter++);
        ActiveVariable.IsPointer = isPointer;
		ActiveVariable.VarType = IRVarType::Variable;
        if (isPointer)
            ActiveVariable.Type = "ptr";
        else
            ActiveVariable.Type = "i64";
    }

    template <typename ...T>
    void Comment(fmt::format_string<T...> format, T&&... args)
    {
        if (!FF_LLVM_VERBOSE) return;
        Expressions.push_back("\t; " + fmt::vformat(format, fmt::make_format_args(args...)));
    }
    void Comment(std::string comment)
    {
        if (!FF_LLVM_VERBOSE) return;
        Expressions.push_back("\t; " + comment);
    }

    template <typename ...T>
    void Operation(fmt::format_string<T...> format, T&&... args)
    {
        Expressions.push_back("\t" + fmt::vformat(format, fmt::make_format_args(args...)));
    }
    void Operation(std::string operation)
    {
        Expressions.push_back("\t" + operation);
    }

    template <typename ...T>
    void Label(fmt::format_string<T...> format, T&&... args)
    {
        Expressions.push_back(fmt::vformat(format, fmt::make_format_args(args...)));
    }

    void Label(std::string label)
    {
        Expressions.push_back(label);
    }
	
	template <typename ...T>
    void Jump(fmt::format_string<T...> format, T&&... args)
    {
		std::string labelName = fmt::vformat(format, fmt::make_format_args(args...));
        Expressions.push_back("\tbr label %" + labelName);
		LastJumpPoint = labelName;
    }
	
	template <typename ...T>
    void Jump_NoUpdate(fmt::format_string<T...> format, T&&... args)
    {
		std::string labelName = fmt::vformat(format, fmt::make_format_args(args...));
        Expressions.push_back("\tbr label %" + labelName);
    }

    void Branch(IRVariableDescriptor condition, std::string if_true, std::string if_false, bool update = true)
    {
        Expressions.push_back(fmt::format("\tbr i1 {}, label %{}, label %{}", condition.GetOnlyName(), if_true, if_false));
		if (update)
		{
			LastJumpPoint = if_false;
		}
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

    virtual ~Expression() = default;

public:
    virtual std::string ToString() { Log::FRONTEND->critical("Pure virtual call to Expression::ToString"); throw std::runtime_error(""); }
    virtual void ToIR(ExpressionStack& stack) { Log::FRONTEND->critical("Pure virtual call to Expression::ToIR"); throw std::runtime_error(""); }

    virtual std::string ToHumanReadableString(std::string depthPrefix)
    {
        return ToString();
    }

    virtual std::vector<std::shared_ptr<Expression>> SubExpressions()
    {
        return std::vector<std::shared_ptr<Expression>>{};
    }

    virtual void ResolveNames(Scope* scope) {};
    virtual void TypeCheck(Scope* scope) {};
};

/// ==========
/// Value Expressions
/// ==========

class ValueExpression : public Expression
{
public:
    Volk::ValueExpressionType ValueExpressionType;
    OperatorArity Arity;

    // After Resolution
    std::shared_ptr<Volk::VKType> ResolvedType;

public:
    ValueExpression(Volk::ValueExpressionType valueType,  OperatorArity arity, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::Value, token)
    {
        ValueExpressionType = valueType;
        Arity = arity;
    }
};
}
