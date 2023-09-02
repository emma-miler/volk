#pragma once

#include <vector>
#include <map>
#include <memory>
#include "type.h"
#include "../log/log.h"
#include "exceptions.h"

#include "expression_base.h"
#include "variable.h"
namespace Volk {

class Scope;

class Scope
{
public:
    std::vector<std::unique_ptr<Expression>> Expressions;
    std::map<std::string, std::shared_ptr<Type>> Types;
    std::map<std::string, std::shared_ptr<Variable>> Variables;

    std::shared_ptr<Scope> ParentScope;

public:
    Scope(std::shared_ptr<Scope> parentScope)
    {
        ParentScope = parentScope;
    }

    void AddType(std::shared_ptr<Type> type)
    {
        Types[type->Name] = std::move(type);
    }
    void AddVariable(std::shared_ptr<Variable> variable)
    {
        Variables[variable->Name] = std::move(variable);
    }

    std::shared_ptr<Variable> FindVariable(std::string name)
    {
        auto it = Variables.find(name);
        if (it == Variables.end())
            return ParentScope->FindVariable(name);
        return it->second;
    }

    std::shared_ptr<Type> FindType(std::string name)
    {
        auto it = Types.find(name);
        if (it == Types.end())
        {
            if (ParentScope == nullptr)
            {
                Log::TYPESYS->critical("Unknown type '{}'", name);
                throw type_error("");
            }
            return ParentScope->FindType(name);
        }
        return it->second;
    }

    void AddBuiltinTypes();
};

}
