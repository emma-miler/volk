#include "../../expression/value_funccall.h"

namespace Volk
{

std::string FunctionCallValueExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = fmt::format("FunctionCallValueExpression");
    out += newline + fmt::format("name={}", FunctionName);
    out += newline + "args=";
    for (auto&& arg : Arguments)
    {
        out += newline + INDENT + arg->ToHumanReadableString(depthPrefix + INDENT + INDENT);
    }
    return out;
}

void FunctionCallValueExpression::ToIR(ExpressionStack& stack)
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
            std::string load = fmt::format("%{} = load {}, {}", stack.ActiveVariable.Name, arg->ResolvedType->LLVMType, variable.Get());
            stack.Operation(load);
        }
        Log::OUTPUT->trace("Has varargs: {}", functionHasVarArgs);
        Log::OUTPUT->trace("Param type: {}", arg->ResolvedType ->Name);
        Log::OUTPUT->trace("Equal: {}", arg->ResolvedType == BUILTIN_FLOAT);
        // This is needed because of the float promotion rule
        if (functionHasVarArgs && arg->ResolvedType == BUILTIN_FLOAT)
        {
            IRVariableDescriptor variable = stack.ActiveVariable;
            stack.AdvanceActive(0);
            std::string load = fmt::format("%{} = fpext float %{} to double", stack.ActiveVariable.Name, variable.Name);
            stack.Operation(load);
            stack.ActiveVariable.Type = "double";
        }
        else
        {
            stack.ActiveVariable.Type = arg->ResolvedType->LLVMType;
        }
        if (arg->ResolvedType->Name == "string")
        {
            stack.ActiveVariable.Type =  "ptr";
        }
        argumentNames.push_back(stack.ActiveVariable);
    }
    stack.Comment("END FUNCTION CALL ARGUMENTS");
    stack.Comment("START FUNCTION CALL");
    stack.AdvanceActive(0);
    std::string ir = fmt::format("%{} = call noundef {} @{}(", stack.ActiveVariable.Name, ResolvedFunction->ReturnType->LLVMType, FunctionName);
    if (argumentNames.size() > 0)
    {
        for (auto&& arg : argumentNames)
        {
            ir += fmt::format("{}, ", arg.Get());
        }
        ir = ir.substr(0, ir.length() - 2);
    }
    ir += ")";
    stack.Operation(ir);
    stack.Comment("END FUNCTION CALL\n");
}

void FunctionCallValueExpression::ResolveNames(Scope* scope)
{
    for (auto&& arg : Arguments)
    {
        arg->ResolveNames(scope);
    }

    std::shared_ptr<Variable> foundVar = scope->FindVariable(FunctionName);
    if (foundVar == nullptr)
    {
        Log::TYPESYS->error("Unknown function '{}'", FunctionName);
        for (auto&& func : scope->Functions)
        {
            Log::TYPESYS->trace("func: '{}'", func.second->ToString());
        }
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

std::vector<Expression*> FunctionCallValueExpression::SubExpressions()
{
    std::vector<Expression*> exprs;
    for (auto&& arg : Arguments)
    {
        exprs.push_back(arg.get());
    }
    return exprs;
}

void FunctionCallValueExpression::TypeCheck(Scope* scope)
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
}
