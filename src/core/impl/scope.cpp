#include "../scope.h"

#include "../variable.h"

namespace Volk
{
void Scope::AddBuiltinTypes()
{
    AddType(BUILTIN_INT);
    AddType(BUILTIN_FLOAT);
    AddType(BUILTIN_DOUBLE);
    AddType(BUILTIN_BOOL);
    AddType(BUILTIN_STRING);
    AddType(BUILTIN_FUNCTION);
    AddType(BUILTIN_VOID);
    // Don't add __var_args, since it's not supposed to be user accessible for now
}

void Scope::AddType(std::shared_ptr<VKType> type)
{
    Types[type->Name] = std::move(type);
}
void Scope::AddVariable(std::shared_ptr<Variable> variable)
{
    Variables[variable->Name] = std::move(variable);
}

std::shared_ptr<Variable> Scope::FindVariable(std::string name)
{
    auto it = Variables.find(name);
    if (it == Variables.end())
    {
        if (ParentScope == nullptr)
            return nullptr;
        return ParentScope->FindVariable(name);
    }
    return it->second;
}

std::shared_ptr<VKType> Scope::FindType(std::string name)
{
    auto it = Types.find(name);
    if (it == Types.end())
    {
        if (ParentScope == nullptr)
            return nullptr;
        return ParentScope->FindType(name);
    }
    return it->second;
}

void Scope::Indicate()
{
    Log::TYPESYS->info("Scope: '{}'", ToString());
}

}
