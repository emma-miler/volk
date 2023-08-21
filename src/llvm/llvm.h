#pragma once

#include <string>
#include "../core/program.h"


namespace Volk
{

class VKLLVM
{
public:
    std::string generateOutput(Program& program);
};

}
