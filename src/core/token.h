#pragma once

#include <string>
#include <string_view>
#include <map>
#include "exceptions.h"

#include "../log/log.h"

#include "operator.h"

namespace Volk
{

enum class TokenType
{
    Dummy,
    Name,
    ImmediateValue,
    Assignment,
    Operator,
    Return,
    EndOfStatement,
    OpenExpressionScope,
    CloseExpressionScope,
    FunctionPrefix,
    OpenScope,
    CloseScope,
    CommaSeperator,
};

static std::map<TokenType, std::string> TokenTypeNames =
{
    {TokenType::Dummy, "DUMMY_INTERNAL"},
    {TokenType::Name, "Name"},
    {TokenType::ImmediateValue, "ImmediateValue"},
    {TokenType::Assignment, "Assignment"},
    {TokenType::Operator, "Operator"},
    {TokenType::Return, "Return"},
    {TokenType::EndOfStatement, "EndOfStatement"},
    {TokenType::OpenExpressionScope, "OpenExpressionScope"},
    {TokenType::CloseExpressionScope, "CloseExpressionScope"},
    {TokenType::FunctionPrefix, "FunctionPrefix"},
    {TokenType::OpenScope, "OpenScope"},
    {TokenType::CloseScope, "CloseScope"},
    {TokenType::CommaSeperator, "CommaSeperator"},
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

public:
    virtual std::string ToString()
    {
        return fmt::format("{}(value='{}')", TokenTypeNames[Type], Value);
    }
};

class OperatorToken : public Token
{
public:
    OperatorType OpType;

public:
    static OperatorToken Dummy()
    {
        return OperatorToken();
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
private:
    OperatorToken() : Token(TokenType::Operator, "", SourcePosition())
    {
        OpType = OperatorType::Null;
    }
};

}
