#pragma once

#include <string>
#include <string_view>
#include <map>

#include "../log/log.h"

#include "operator.h"

namespace Volk
{

enum class TokenType
{
    Name,
    ImmediateValue,
    Assignment,
    Operator,
    Return,
    EndOfStatement,
};

static std::map<TokenType, std::string> TokenTypeNames =
{
    {TokenType::Name, "Name"},
    {TokenType::ImmediateValue, "ImmediateValue"},
    {TokenType::Assignment, "Assignment"},
    {TokenType::Operator, "Operator"},
    {TokenType::Return, "Return"},
    {TokenType::EndOfStatement, "EndOfStatement"}
};

typedef struct
{
    int LineIndex;
    int LineOffset;
    int Length;
} SourcePosition;

class Token
{
public:
    TokenType Type;
    std::string Value;
    SourcePosition Position;

public:
    Token(TokenType type, std::string_view value, SourcePosition position)
    {
        Type = type;
        Value = std::string(value);
        Position = position;
    }
};

class OperatorToken : public Token
{
public:
    OperatorType OpType;

public:
    OperatorToken(TokenType type, std::string_view value, SourcePosition position) : Token(type, value, position)
    {
        auto optype = OperatorTypeLookup.find(std::string(value));
        if (optype == OperatorTypeLookup.end())
        {
            Log::LEXER->critical("Failed to parse operator type. This should never happen!");
            Log::LEXER->critical("Operator value: '{}'", value);
            throw std::format_error("");
        }
        OpType = optype->second;
    }
};

}
