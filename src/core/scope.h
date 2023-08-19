#pragma once

#include <vector>
#include <map>
#include <memory>
#include "type.h"

#include "expression_base.h"
#include "variable.h"
namespace Volk {

class Scope
{
public:
    std::vector<std::unique_ptr<Expression>> Expressions;
    std::map<std::string, std::unique_ptr<Type>> Types;
    std::map<std::string, std::unique_ptr<Variable>> Variables;

public:
    Scope() {}

    void AddType(std::unique_ptr<Type> type)
    {
        Types[type->Name] = std::move(type);
    }
    void AddVariable(std::unique_ptr<Variable> variable)
    {
        Variables[variable->Name] = std::move(variable);
    }

    void AddBuiltinTypes();
};

}
