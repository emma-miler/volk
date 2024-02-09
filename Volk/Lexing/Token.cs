using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
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
    public OperatorType OperatorType { get; }
    public OperatorToken(OperatorType valueType, SourcePosition valueSource) : base(TokenType.Operator, valueSource)
    {
        OperatorType = valueType;
    }

    public override string ToString()
    {
        return $"{Type} ({OperatorType}) {ValueSource}";
    }

    public string GetIROperator() => GetIROperator(OperatorType);

    public static string GetIROperator(OperatorType op)
    {
        switch (op)
        {
            case OperatorType.Add: return "add";
            case OperatorType.Sub: return "sub";
            case OperatorType.Mul: return "mul";
            case OperatorType.Div: return "div";
            case OperatorType.Mod: return "rem";
            case OperatorType.Eq: return "eq";
            case OperatorType.Ne: return "ne";
            case OperatorType.Gt: return "gt";
            case OperatorType.Ge: return "ge";
            case OperatorType.Lt: return "lt";
            case OperatorType.Le: return "le";
            case OperatorType.UnaryDecrement: return "sub";
            case OperatorType.UnaryIncrement: return "add";
            default: throw new InvalidEnumArgumentException($"Invalid operator '{op}'");
        }
    }

    public bool IsComparisonOperator => OperatorType >= OperatorType.Eq && OperatorType <= OperatorType.Le;
    public bool IsUnaryOperator => OperatorType == OperatorType.Sub || OperatorType == OperatorType.Neg || OperatorType == OperatorType.UnaryIncrement || OperatorType == OperatorType.UnaryDecrement;
}