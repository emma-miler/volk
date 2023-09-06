#include "../function.h"

namespace Volk
{

FunctionObject::FunctionObject(std::string name, std::shared_ptr<VKType> returnType, std::vector<std::shared_ptr<FunctionParameter>> parameters, std::shared_ptr<Scope> parentScope) : Variable(name, BUILTIN_FUNCTION)
{
    FunctionScope = std::make_shared<Scope>(parentScope, returnType);
    ReturnType = returnType;
    Parameters = parameters;
    for (auto&& param : Parameters)
    {
        FunctionScope->Variables[param->Name] = param;
    }
}

}
