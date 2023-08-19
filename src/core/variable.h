#pragma once

#include "object.h"
#include "type.h"

namespace Volk
{
class Variable : public Object
{
public:
    std::unique_ptr<Type> VarType;

    Variable(std::string name, std::unique_ptr<Type> type) : Object(name)
    {
        VarType = std::move(type);
    }
};
}
