#pragma once
#include "../token.h"

namespace Volk
{

class StringToken : public Token
{
public:
    StringToken(std::string_view value, SourcePosition position, std::vector<std::string>& table) : Token(TokenType::StringConstant, "", position)
    {
        Value = std::to_string(table.size());
        table.push_back(string_desanitize(std::string(value)));
    }
};
}
