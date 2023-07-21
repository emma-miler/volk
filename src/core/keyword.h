#pragma once

#include <string>
#include <array>

namespace Volk
{
constexpr std::array<std::string, 1> Keywords =
{
    "return"
};


static std::map<std::string, TokenType> KeywordLookup =
{
    {"return", TokenType::Return}
};

}
