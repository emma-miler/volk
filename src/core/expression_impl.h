#pragma once
#include "expression_base.h"

#include "variable.h"

namespace Volk
{

class ImmediateValueExpression : public ValueExpression
{
public:
    std::string Value;

public:
    ImmediateValueExpression(std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::Immediate, OperatorArity::Nullary, token)
    {
        Value = token->Value;
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
        stack.Comment("START IMMEDIATE VALUE");
        stack.Expressions.push_back(fmt::format("%{} = alloca i64", stack.ActiveVariable.Name));
        // Assign the value
        stack.Expressions.push_back(fmt::format("store i64 {}, {}", Value, stack.ActiveVariable.Get()));
        stack.Comment("END IMMEDIATE VALUE\n");

    }
};

class IndirectValueExpression : public ValueExpression
{
public:
    std::string Value;
    std::shared_ptr<Volk::Variable> Variable;

public:
    IndirectValueExpression(std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::Indirect, OperatorArity::Nullary, token)
    {
        Value = token->Value;
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
        // If it is already the most recent value on the stack, we don't need to do anything
        if (Value == stack.ActiveVariable.Name) return;
        stack.AdvanceActive(0);
        std::string variableName = stack.ActiveVariable.Name;
        stack.Comment("START INDIRECT VALUE");
        // Assign the value
        // TODO: figure out the type of Value here
        stack.Expressions.push_back(fmt::format("%{} = load {}, ptr %{}", variableName, Variable->Type->IsReferenceType ? "ptr" : "i64", Value));
        stack.Comment("END INDIRECT VALUE\n");
    }
};

class StringConstantValueExpression : public ValueExpression
{
public:
    std::string Index;

public:
    StringConstantValueExpression(std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::StringConstant, OperatorArity::Nullary, token)
    {
        Index = token->Value;
    }

    std::string ToString()
    {
        return fmt::format("StringConstantValueExpression(index='{}')", Index);
    }

    std::string ToHumanReadableString(std::string depthPrefix)
    {
        return fmt::format("StringConstantValueExpression(index={})", Index);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        // We dont want to actually advance the counter here,
        // since we're not actually pushing anything new
        stack.AdvanceActive(1);
        stack.ActiveVariable.IsConstant = 1;
        stack.ActiveVariable.Name = fmt::format(".str.{}", Index);
        stack.NameCounter--;
        stack.Comment("PUSHED STRING CONSTANT VALUE");
    }
};

class UnaryValueExpression : public ValueExpression
{
public:
    std::unique_ptr<Expression> Value;
    OperatorType Operator;

public:
    UnaryValueExpression(OperatorType op, std::unique_ptr<Expression> value, std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::Unary, OperatorArity::Unary, token)
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
        stack.Comment("START UNARY OPERATOR");
        if (stack.ActiveVariable.IsPointer)
        {
            stack.AdvanceActive(0);
            stack.Expressions.push_back(fmt::format("%{} = load i64, ptr %{}", stack.ActiveVariable.Name, valueVariableName));
            valueVariableName = stack.ActiveVariable.Name;
        }
        stack.AdvanceActive(0);
        stack.Expressions.push_back(fmt::format("%{} = {} nsw i64 0, %{}", stack.ActiveVariable.Name, Operator == OperatorType::OperatorMinus ? "sub" : "add", valueVariableName));
        stack.Comment("END UNARY OPERATOR\n");
    }

    std::vector<Expression*> SubExpressions()
    {
        return std::vector<Expression*>{ Value.get() };
    }
};

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

class FunctionCallValueExpression : public ValueExpression
{
public:
    std::string FunctionName;
    std::vector<std::unique_ptr<ValueExpression>> Arguments;

public:
    FunctionCallValueExpression(std::string functionName, std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::FunctionCall, OperatorArity::Nullary, token)
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
        stack.Comment("START FUNCTION CALL ARGUMENTS");
        for (auto&& arg : Arguments)
        {
            arg->ToIR(stack);
            if (!arg->VariableType->IsReferenceType && stack.ActiveVariable.IsPointer)
            {
                IRVariableDescriptor variable = stack.ActiveVariable;
                stack.AdvanceActive(0);
                std::string load = fmt::format("%{} = load i64, {}", stack.ActiveVariable.Name, variable.Get());
                stack.Expressions.push_back(load);
            }
            argumentNames.push_back(stack.ActiveVariable);
        }
        stack.Comment("END FUNCTION CALL ARGUMENTS");
        stack.Comment("START FUNCTION CALL");
        stack.AdvanceActive(0);
        std::string ir = fmt::format("%{} = call noundef i64 @{}(", stack.ActiveVariable.Name, FunctionName);
        for (auto&& arg : argumentNames)
        {
            ir += fmt::format("{}, ", arg.Get());
        }
        ir = ir.substr(0, ir.length() - 2);
        ir += ")";
        stack.Expressions.push_back(ir);
        stack.Comment("END FUNCTION CALL\n");
    }

     std::vector<Expression*> SubExpressions()
    {
        std::vector<Expression*> exprs {};
        for (auto&& arg : Arguments)
        {
            exprs.push_back(arg.get());
        }
        return exprs;
    }

};








class DeclarationExpression : public Expression
{
public:
    std::string Typename;
    std::string Name;

public:
    DeclarationExpression(std::string type, std::string name, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::Declaration, token)
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
        stack.Comment("START DECLARATION");
        stack.Expressions.push_back(fmt::format("%{} = alloca {}", Name, Typename == "string" ? "ptr" : "i64"));
        stack.Comment("END DECLARATION\n");
    }
};

class AssignmentExpression : public Expression
{
public:
    std::string Name;
    std::unique_ptr<ValueExpression> Value;

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
         return fmt::format("AssignmentExpression(\n{}\tname='{}', \n{}\tvalue={}\n{})", depthPrefix, Name, depthPrefix, Value->ToHumanReadableString(depthPrefix + "\t"), depthPrefix);
    }

    virtual void ToIR(ExpressionStack& stack)
    {
        if (Value->ValueExpressionType == ValueExpressionType::Immediate)
        {
            ImmediateValueExpression value = *static_cast<ImmediateValueExpression*>(Value.get());
            stack.Comment("START ASSIGNMENT");
            stack.Expressions.push_back(fmt::format("store i64 {}, ptr %{}", value.Value, Name));
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
};

class ReturnExpression : public Expression
{
public:
    std::unique_ptr<ValueExpression> Value;

public:
    ReturnExpression(std::unique_ptr<ValueExpression> value, std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::ReturnExpr, token)
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
        stack.Comment("START RETURN VALUE");
        Value->ToIR(stack);
        stack.Comment("END RETURN VALUE");
        stack.Comment("START RETURN");
        if (stack.ActiveVariable.IsPointer)
        {
            IRVariableDescriptor variable = stack.ActiveVariable;
            stack.AdvanceActive(0);
            stack.Expressions.push_back(fmt::format("%{} = load i64, {}", stack.ActiveVariable.Name, variable.Get()));
            stack.Expressions.push_back(fmt::format("ret i64 %{}", stack.ActiveVariable.Name));
        }
        else
        {
            stack.Expressions.push_back(fmt::format("ret i64 %{}", stack.ActiveVariable.Name));
        }
        stack.Comment("END RETURN\n");
    }

    std::vector<Expression*> SubExpressions()
    {
        return std::vector<Expression*>{ Value.get() };
    }
};


}
