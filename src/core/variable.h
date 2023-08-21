#pragma once

#include "object.h"
#include "type.h"

namespace Volk
{
class Variable : public Object
{
public:
    std::shared_ptr<Type> Type;

    Variable(std::string name, std::shared_ptr<Volk::Type> type) : Object(name)
    {
        this->Type = type;
    }
};
}
