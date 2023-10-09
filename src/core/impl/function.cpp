#include "../function.h"

namespace Volk
{

FunctionObject::FunctionObject(std::string name, std::shared_ptr<VKType> returnType, std::vector<std::shared_ptr<FunctionParameter>> parameters, std::shared_ptr<Scope> parentScope) : Variable(name, BUILTIN_FUNCTION)
{
    FunctionScope = std::make_shared<Scope>(name, parentScope, returnType);
    ReturnType = returnType;
    Parameters = parameters;
    for (auto&& param : Parameters)
    {
        FunctionScope->Variables[param->Name] = param;
    }
}


void FunctionObject::ToIR(ExpressionStack& stack)
{
    std::string definitionString = fmt::format("define dso_local noundef {} @{}(", ReturnType->LLVMType, Name);
    for (auto&& param : Parameters)
    {
        definitionString += fmt::format("{} noundef %param.{}, ", param->Type->LLVMType, param->Name);
    }
    definitionString = definitionString.substr(0, definitionString.length() - 2);
    definitionString += ") #0\n{";
    stack.Label(definitionString);
    ExpressionStack innerStack;
    for (auto&& param : Parameters)
    {
        innerStack.Operation(fmt::format("%{} = alloca {}, align 4", param->Name, param->Type->LLVMType));
        innerStack.Operation(fmt::format("store {} %param.{}, ptr %{}, align 4", param->Type->LLVMType, param->Name, param->Name));
    }
    for (auto&& expr : FunctionScope->Expressions)
    {
        expr->ToIR(innerStack);
    }
    for (std::string& line : innerStack.Expressions)
    {
        stack.Label(line);
    }
    stack.Label("}");

    for (auto&& subFunc : FunctionScope->Functions)
    {
        subFunc.second->ToIR(stack);
    }
}

}
