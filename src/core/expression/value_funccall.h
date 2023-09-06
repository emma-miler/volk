#pragma once
#include "../expression_base.h"
#include "../function.h"

namespace Volk
{

class FunctionCallValueExpression : public ValueExpression
{
public:
    std::string FunctionName;
    std::vector<std::unique_ptr<ValueExpression>> Arguments;


    std::shared_ptr<FunctionObject> ResolvedFunction;

public:
    FunctionCallValueExpression(std::string functionName, std::shared_ptr<Volk::Token> token) : ValueExpression(ValueExpressionType::FunctionCall, OperatorArity::Nullary, token)
    {
        FunctionName = functionName;
    }

    std::string ToString()
    {
        return fmt::format("FunctionCallValueExpression(name='{}')", FunctionName);
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
        bool functionHasVarArgs = false;
        for (auto&& param : ResolvedFunction->Parameters)
        {
            if (param->Type->Name == "__var_args")
            {
                functionHasVarArgs = true;
                break;
            }
        }
        stack.Comment("START FUNCTION CALL ARGUMENTS");
        for (auto&& arg : Arguments)
        {
            arg->ToIR(stack);
            if (!arg->ResolvedType->IsReferenceType && stack.ActiveVariable.IsPointer)
            {
                IRVariableDescriptor variable = stack.ActiveVariable;
                stack.AdvanceActive(0);
                std::string load = fmt::format("%{} = load i64, {}", stack.ActiveVariable.Name, variable.Get());
                stack.Expressions.push_back(load);
            }
            Log::TYPESYS->debug("Has varargs: {}", functionHasVarArgs);
            Log::TYPESYS->debug("Param type: {}", arg->ResolvedType ->Name);
            Log::TYPESYS->debug("Equal: {}", arg->ResolvedType == BUILTIN_FLOAT);
            // This is needed because of the float promotion rule
            if (functionHasVarArgs && arg->ResolvedType == BUILTIN_FLOAT)
            {
                IRVariableDescriptor variable = stack.ActiveVariable;
                stack.AdvanceActive(0);
                std::string load = fmt::format("%{} = fpext float %{} to double", stack.ActiveVariable.Name, variable.Name);
                stack.Expressions.push_back(load);
                stack.ActiveVariable.Type = "double";
            }
            argumentNames.push_back(stack.ActiveVariable);
        }
        stack.Comment("END FUNCTION CALL ARGUMENTS");
        stack.Comment("START FUNCTION CALL");
        stack.AdvanceActive(0);
        std::string ir = fmt::format("%{} = call noundef i64 @{}(", stack.ActiveVariable.Name, FunctionName);
        if (argumentNames.size() > 0)
        {
            for (auto&& arg : argumentNames)
            {
                ir += fmt::format("{}, ", arg.Get());
            }
            ir = ir.substr(0, ir.length() - 2);
        }
        ir += ")";
        stack.Expressions.push_back(ir);
        stack.Comment("END FUNCTION CALL\n");
    }

    virtual void ResolveNames(Scope* scope)
    {
        for (auto&& arg : Arguments)
        {
            arg->ResolveNames(scope);
        }

        std::shared_ptr<Variable> foundVar = scope->FindVariable(FunctionName);
        if (foundVar == nullptr)
        {
            Log::TYPESYS->error("Unknown function '{}'", FunctionName);
            Token->Indicate();
            throw type_error("");
        }
        if (foundVar->Type != BUILTIN_FUNCTION)
        {
            Log::TYPESYS->error("Object '{}: {}' is not a function", FunctionName, foundVar->Type->Name);
            Token->Indicate();
            throw type_error("");
        }
        ResolvedFunction = std::static_pointer_cast<FunctionObject>(foundVar);
        ResolvedType = ResolvedFunction->ReturnType;
    }

    std::vector<Expression*> SubExpressions()
    {
        std::vector<Expression*> exprs;
        for (auto&& arg : Arguments)
        {
            exprs.push_back(arg.get());
        }
        return exprs;
    }

    virtual void TypeCheck(Scope* scope)
    {
        int i = 0;
        for (auto&& arg : Arguments)
        {
            if (ResolvedFunction->Parameters[i]->Type == BUILTIN_VARARGS)
            {
                // If we hit a varargs, it doesnt make sense to continue searching, cause we cant guarantee type safety anymore anyway
                return;
            }
            if (arg->ResolvedType != ResolvedFunction->Parameters[i]->Type)
            {
                Log::TYPESYS->error("Cannot implicitly convert from '{}' to '{}'", arg->ResolvedType->Name, ResolvedFunction->Parameters[i]->Type->Name);
                Token->Indicate();
                throw type_error("");
            }
            i++;
        }

    }

};

}
