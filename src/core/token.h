#pragma once
#include <string>
#include <string_view>
#include <map>
#include "exceptions.h"
#include "../util/string.h"

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
    StringConstant,
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
    {TokenType::StringConstant, "StringConstant"},
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

    virtual void Indicate(std::vector<std::string>& lines)
    {
        Log::PARSER->info("{}", lines[Position.LineIndex]);
        std::string space = fmt::format("{: >{}}", ' ', Position.LineOffset);
        std::string tokenIndicator = fmt::format("{:^>{}}", ' ', Position.Length + 1);
        Log::PARSER->info(space + tokenIndicator);
    }
};

class OperatorToken : public Token
{
public:
    OperatorType OpType;

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
