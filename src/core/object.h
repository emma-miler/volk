#pragma once

#include <string>
#include <memory>

namespace Volk {

class Object
{
public:
    std::string Name;

public:
    Object(std::string name)
    {
        Name = name;
    }
};

}
