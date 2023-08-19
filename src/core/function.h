#pragma once

#include <vector>
#include <memory>

#include "object.h"
#include "scope.h"

namespace Volk {

class FunctionParameter : public Object
{
public:
    std::string Type;
    bool IsOptional;

public:
    FunctionParameter(std::string name, std::string type) : Object(name)
    {
        Type = type;
        IsOptional = false;
    }
};

class FunctionObject : public Object
{
public:
    std::string Type;
    std::vector<FunctionParameter> Parameters;
    std::shared_ptr<Scope> FunctionScope;

public:
    FunctionObject(std::string name, std::string type) : Object(name)
    {
        FunctionScope = std::make_shared<Scope>();
        Type = type;
    }

    std::string ToHumanReadable()
    {
        std::string value = fmt::format("\nFunction {} {}(\nargs=[", Type, Name);
        for (auto&& param : Parameters)
        {
            value += fmt::format("\n\t{} {}, ", param.Type, param.Name);
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
