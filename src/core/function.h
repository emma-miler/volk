#pragma once

#include <vector>
#include <memory>
#include <string>

namespace Volk
{
    class FunctionObject;
}

#include "object.h"
#include "scope.h"
#include "variable.h"

namespace Volk
{

class FunctionParameter : public Variable
{
public:
    bool IsOptional;

public:
    FunctionParameter(std::string name, std::shared_ptr<VKType> type) : Variable(name, type)
    {
        Type = type;
        IsOptional = false;
    }
};

class FunctionObject : public Variable
{
public:
    std::shared_ptr<VKType> ReturnType;
    std::vector<FunctionParameter> Parameters;
    std::shared_ptr<Scope> FunctionScope;

public:
    FunctionObject(std::string name, std::shared_ptr<VKType> returnType, std::shared_ptr<Scope> parentScope) : Variable(name, BUILTIN_FUNCTION)
    {
        FunctionScope = std::make_shared<Scope>(parentScope);
        ReturnType = returnType;
    }

    std::string ToHumanReadable()
    {
        std::string value = fmt::format("\nFunction {} {}(\nargs=[", Type->Name, Name);
        for (auto&& param : Parameters)
        {
            value += fmt::format("\n\t{} {}, ", param.Type->Name, param.Name);
        }
        value = value.substr(0, value.length() - 2);
        value += "\n)";
        return value;
    }

    std::string ToIR()
    {
        std::string value = fmt::format("define dso_local noundef i32 @{}(", Name);
        for (auto&& param : Parameters)
        {
            value += fmt::format("ptr noundef %{}, ", param.Name);
        }
        value = value.substr(0, value.length() - 2);
        value += ")";
        return value;
    }
};

}
