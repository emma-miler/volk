#pragma once
#include "../common.h"
#include "exceptions.h"
#include "../util/string.h"
#include "operator.h"
#include "sourceposition.h"

namespace Volk
{

enum class TokenType
{
    Dummy,
    Name,
    ImmediateIntValue,
    ImmediateFloatValue,
    ImmediateDoubleValue,
    ImmediateBoolValue,
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
    {TokenType::ImmediateIntValue, "ImmediateIntValue"},
    {TokenType::ImmediateFloatValue, "ImmediateFloatValue"},
    {TokenType::ImmediateDoubleValue, "ImmediateDoubleValue"},
    {TokenType::ImmediateBoolValue, "ImmediateBoolValue"},
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
    virtual std::string ToString();
    virtual void Indicate();

    virtual ~Token();
};




}
