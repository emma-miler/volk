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
    {"function", TokenType::FunctionPrefix},
	{"true", TokenType::ImmediateBoolValue},
    {"false", TokenType::ImmediateBoolValue},
    {"if", TokenType::IfStatement},
    {"else", TokenType::ElseStatement},
    {"for", TokenType::ForStatement},
};

}
