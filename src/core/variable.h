#pragma once

#include "object.h"
#include "type.h"

namespace Volk
{
class Variable : public Object
{
public:
    std::unique_ptr<Type> Type;

    Variable(std::string name, std::unique_ptr<Volk::Type> type) : Object(name)
    {
        this->Type = std::move(type);
    }
};
}
