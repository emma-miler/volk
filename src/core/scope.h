#pragma once

#include "../common.h"

namespace Volk
{
    class Scope;
}

#include "type.h"
#include "variable.h"

#include "expression_base.h"
#include "function.h"

namespace Volk
{
class Scope : public Object
{
public:
    std::vector<std::unique_ptr<Expression>> Expressions;
    std::map<std::string, std::shared_ptr<VKType>> Types;
    std::map<std::string, std::shared_ptr<Variable>> Variables;
    std::map<std::string, std::shared_ptr<FunctionObject>> Functions;

    std::shared_ptr<VKType> ReturnType;

    std::shared_ptr<Scope> ParentScope;

public:
    Scope(std::string name, std::shared_ptr<Scope> parentScope, std::shared_ptr<VKType> returnType) : Object(name)
    {
        ParentScope = parentScope;
        ReturnType = returnType;
    }

    void AddType(std::shared_ptr<VKType> type);
    void AddVariable(std::shared_ptr<Variable> variable);

    std::shared_ptr<Variable> FindVariable(std::string name);
    std::shared_ptr<VKType> FindType(std::string name);

    void AddBuiltinTypes();

    void Indicate();
};
}
