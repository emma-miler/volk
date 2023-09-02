#pragma once

#include "object.h"
#include "type.h"

namespace Volk
{
class Variable : public Object
{
public:
    std::shared_ptr<VKType> Type;

    Variable(std::string name, std::shared_ptr<VKType> type) : Object(name)
    {
        this->Type = type;
    }
};
}
