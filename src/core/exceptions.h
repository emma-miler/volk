#pragma once

#include <stdexcept>

namespace Volk
{

class parse_error : public std::runtime_error
{
    std::string what_message;
public:
    parse_error(std::string message) : std::runtime_error(message)
    {

    }
    const char* what()
    {
        return what_message.c_str();
    }
};

}
