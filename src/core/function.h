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
#include "expression_base.h"
#include "../util/options.h"

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
    std::vector<std::shared_ptr<FunctionParameter>> Parameters;
    std::shared_ptr<Scope> FunctionScope;

public:
    FunctionObject(std::string name, std::shared_ptr<VKType> returnType, std::vector<std::shared_ptr<FunctionParameter>> parameters, std::shared_ptr<Scope> parentScope);

    std::string ToHumanReadable();

    void ToIR(ExpressionStack& stack);
};

}
