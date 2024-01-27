using Volk.Core.Objects;
using Volk.Core.Values;

namespace Volk.Core;

public abstract class Expression
{
    public ExpressionType ExpressionType { get; }
    public Token Token { get; }

    public Expression(ExpressionType type, Token token)
    {
        ExpressionType = type;
        Token = token;
    }

    public abstract void Print(int depth);

    /// <summary>
    /// Resolve all string references to objects
    /// </summary>
    /// <param name="scope"></param>
    public abstract void ResolveNames(Scope scope);

    //public abstract void TypeCheck(Scope scope);
}

public abstract class ValueExpression : Expression
{
    public ValueExpressionType ValueExpressionType { get; }

    /// <summary>
    /// NOTE: This property is only available after type resolution 
    /// </summary>
    /// <value></value>
    public VKType? ValueType { get; protected set; }

    public ValueExpression(ValueExpressionType type, Token token) : base(ExpressionType.Value, token)
    {
        ValueExpressionType = type;
    }
}