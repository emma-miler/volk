using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Volk.Core;

namespace Volk.Core;
public class Token
{
    public TokenType Type { get; }
    public SourcePosition Position { get; }
    public string Value { get; }

    public Token(TokenType type, Stream stream, int currentPosition, int length)
    {
        Type = type;
        Position = new SourcePosition(currentPosition - length, length);
        Value = Position.GetValue(stream);
    }

    public Token(TokenType type, string value, int currentPosition, int length)
    {
        Type = type;
        Value = value;
        Position = new SourcePosition(currentPosition - length, length);
    }

    public override string ToString()
    {
        return $"{Type} {Position}";
    }
}

public class ValueToken : Token
{
    public ValueTokenType ValueType { get; }
    public ValueToken(ValueTokenType valueType, Stream stream, int currentPosition, int length) : base(TokenType.ImmediateValue, stream, currentPosition, length)
    {
        ValueType = valueType;
    }

    public ValueToken(ValueTokenType valueType, string value, int currentPosition, int length) : base(TokenType.ImmediateValue, value, currentPosition, length)
    {
        ValueType = valueType;
    }

    public override string ToString()
    {
        return $"{Type} ({ValueType}) {Position}";
    }
}

public class OperatorToken : Token
{
    public OperatorTokenType OperatorType { get; }
    public OperatorToken(OperatorTokenType valueType, Stream stream, int currentPosition, int length) : base(TokenType.Operator, stream, currentPosition, length)
    {
        OperatorType = valueType;
    }
    public OperatorToken(OperatorTokenType valueType, string value, int currentPosition, int length) : base(TokenType.Operator, value, currentPosition, length)
    {
        OperatorType = valueType;
    }

    public override string ToString()
    {
        return $"{Type} ({OperatorType}) {Position}";
    }

    public bool IsComparisonOperator => OperatorType >= OperatorTokenType.Eq && OperatorType <= OperatorTokenType.Le;
    public bool IsUnaryOperator => OperatorType == OperatorTokenType.Minus || OperatorType <= OperatorTokenType.Negate;
}