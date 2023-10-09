#pragma once

namespace Volk
{
    class Token;
}

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
	Comment,
    ImmediateIntValue,
    ImmediateFloatValue,
    ImmediateDoubleValue,
    ImmediateBoolValue,
    EqualSign,
    Operator,
    Return,
    EndOfStatement,
    OpenParenthesis,
    CloseParenthesis,
    OpenAngleBrace,
    CloseAngleBrace,
    FunctionPrefix,
    OpenCurlyBrace,
    CloseCurlyBrace,
    CommaSeperator,
    StringConstant,
    IfStatement,
    ElseStatement,
    ExclamationMark,
    QuestionMark,
    ForStatement,
};

static std::map<TokenType, std::string> TokenTypeNames =
{
    {TokenType::Dummy, "DUMMY_INTERNAL"},
    {TokenType::Name, "Name"},
	{TokenType::Comment, "Comment"},
    {TokenType::ImmediateIntValue, "ImmediateIntValue"},
    {TokenType::ImmediateFloatValue, "ImmediateFloatValue"},
    {TokenType::ImmediateDoubleValue, "ImmediateDoubleValue"},
    {TokenType::ImmediateBoolValue, "ImmediateBoolValue"},
    {TokenType::EqualSign, "EqualSign"},
    {TokenType::Operator, "Operator"},
    {TokenType::Return, "Return"},
    {TokenType::EndOfStatement, "EndOfStatement"},
    {TokenType::OpenParenthesis, "OpenParenthesis"},
    {TokenType::CloseParenthesis, "CloseParenthesis"},
    {TokenType::OpenAngleBrace, "OpenAngledBrace"},
    {TokenType::CloseAngleBrace, "CloseAngledBrace"},
    {TokenType::FunctionPrefix, "FunctionPrefix"},
    {TokenType::OpenCurlyBrace, "OpenCurlyBrace"},
    {TokenType::CloseCurlyBrace, "CloseCurlyBrace"},
    {TokenType::CommaSeperator, "CommaSeperator"},
    {TokenType::StringConstant, "StringConstant"},
    {TokenType::IfStatement, "IfStatement"},
    {TokenType::ElseStatement, "ElseStatement"},
    {TokenType::ExclamationMark, "ExclamationMark"},
    {TokenType::QuestionMark, "QuestionMark"},
    {TokenType::ForStatement, "ForStatement"},
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

    virtual ~Token() = default;
};




}
