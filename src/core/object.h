#pragma once

#include <string>

namespace Volk
{
class Object
{
public:
    std::string Name;

public:
    Object(std::string name)
    {
        Name = name;
    }
    virtual ~Object();

    virtual std::string ToString()
    {
        return Name;
    }
};
}
