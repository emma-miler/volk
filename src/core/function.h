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
};

}
