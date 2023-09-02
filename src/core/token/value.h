#pragma once
#include "../token.h"
#include "../type.h"
#include "../exceptions.h"

namespace Volk
{

class ValueToken : public Token
{
public:
    std::shared_ptr<VKType> ValueType;
public:
    ValueToken(std::string_view value, SourcePosition position, TokenType tokenType) : Token(tokenType, value, position)
    {
        switch (tokenType)
        {
            case TokenType::ImmediateIntValue: ValueType = BUILTIN_INT; break;
            case TokenType::ImmediateFloatValue: ValueType = BUILTIN_FLOAT; break;
            case TokenType::ImmediateDoubleValue: ValueType = BUILTIN_DOUBLE; break;
            case TokenType::ImmediateBoolValue: ValueType = BUILTIN_BOOL; break;
            default: throw parse_error("Unknown token type");
        }
    }
};
}
