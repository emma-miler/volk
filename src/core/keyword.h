#pragma once

#include <string>
#include <array>

namespace Volk
{
const std::array<std::string, 2> Keywords =
{
    "return",
    "func"
};


static std::map<std::string, TokenType> KeywordLookup =
{
    {"return", TokenType::Return},
    {"func", TokenType::FunctionPrefix}
};

}
