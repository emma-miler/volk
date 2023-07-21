#pragma once

#include <string>
#include "../core/token.h"
#include "../parsing/parser.h"


namespace Volk
{

class VKLLVM
{
public:
    std::string generateOutput(VKParser& parser);
};

}
