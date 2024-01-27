using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Volk.Core;
using Volk.Core.Objects;

namespace Volk.Core;
public class Token
{

    public TokenType Type { get; }
    public SourcePosition ValueSource { get; }
    protected string? _value;
    public string Value => _value ??= ValueSource.GetValue(); 

    public Token(TokenType type, SourcePosition valueSource)
    {
        Type = type;
        ValueSource = valueSource;
    }

    public override string ToString()
    {
        return $"{Type} {ValueSource}";
    }
}

public class ValueToken : Token
{
    public VKType ValueType { get; }
    public ValueToken(VKType valueType, SourcePosition valueSource) : base(TokenType.ImmediateValue, valueSource)
    {
        ValueType = valueType;
    }

    public override string ToString()
    {
        return $"{Type} ({ValueType}) {ValueSource}";
    }
}

public class OperatorToken : Token
{
    public OperatorTokenType OperatorType { get; }
    public OperatorToken(OperatorTokenType valueType, SourcePosition valueSource) : base(TokenType.Operator, valueSource)
    {
        OperatorType = valueType;
    }

    public override string ToString()
    {
        return $"{Type} ({OperatorType}) {ValueSource}";
    }

    public bool IsComparisonOperator => OperatorType >= OperatorTokenType.Eq && OperatorType <= OperatorTokenType.Le;
    public bool IsUnaryOperator => OperatorType == OperatorTokenType.Minus || OperatorType == OperatorTokenType.Negate || OperatorType == OperatorTokenType.UnaryIncrement || OperatorType == OperatorTokenType.UnaryDecrement;
}