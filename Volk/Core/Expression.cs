namespace Volk.Core;

public abstract class Expression
{
    public ExpressionType Type { get; }
    public Token Token { get; }

    public Expression(ExpressionType type, Token token)
    {
        Type = type;
        Token = token;
    }

    public abstract void Print(int depth);
}

public abstract class ValueExpression : Expression
{
    public ValueExpressionType ValueType { get; }

    public ValueExpression(ValueExpressionType type, Token token) : base(ExpressionType.Value, token)
    {
        ValueType = type;
    }
}