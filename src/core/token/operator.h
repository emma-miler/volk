#pragma once
#include "../token.h"

namespace Volk
{

class OperatorToken : public Token
{
public:
    OperatorType OpType;
    bool IsComparator;

public:
    static std::shared_ptr<OperatorToken> Dummy()
    {
        return std::make_shared<OperatorToken>();
    }
    OperatorToken(std::string_view value, SourcePosition position) : Token(TokenType::Operator, value, position)
    {
        auto optype = OperatorTypeLookup.find(std::string(value));
        if (optype == OperatorTypeLookup.end())
        {
            Log::LEXER->critical("Failed to parse operator type. This should never happen!");
            Log::LEXER->critical("Operator value: '{}'", value);
            throw parse_error("");
        }
        OpType = optype->second;
        Log::LEXER->trace("Assigning operator token type '{}' for value '{}'", OperatorTypeNames[OpType], value);
    }
public:
    OperatorToken() : Token(TokenType::Operator, "", SourcePosition())
    {
        OpType = OperatorType::Null;
    }
};
}
