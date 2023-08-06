#pragma once

#include <vector>
#include <memory>

#include "function.h"

namespace Volk {

class Namespace
{
public:
    std::string Name;
    std::vector<std::unique_ptr<FunctionObject>> Functions;

public:
    Namespace(std::string name)
    {
        Name = name;
    }
};

}
