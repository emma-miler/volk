namespace Volk.Core;
public enum TokenType
{
    EOF = -1,
    Dummy = 0,
    Name,
    Comment,
    ImmediateValue,
    Operator,
    EndOfExpression,
    OpenParenthesis,
    CloseParenthesis,
    OpenCurlyBracket,
    CloseCurlyBracket,
    CommaSeparator,
    If,
    Else,
    For,
    Return,
    FunctionDeclaration,
}