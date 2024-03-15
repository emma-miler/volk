using Volk.Core.Objects;

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
    public abstract void ResolveNames(VKScope scope);

    /// <summary>
    /// Type-check this expression
    /// </summary>
    public abstract void TypeCheck(VKScope scope);

    /// <summary>
    /// Run code generation for this expression
    /// </summary>
    public abstract IRVariable GenerateCode(CodeGenerator gen);
}

public abstract class ValueExpression : Expression
{
    public ValueExpressionType ValueExpressionType { get; }

    /// <summary>
    /// NOTE: This property is only available after type resolution 
    /// </summary>
    /// <value></value>
    public VKType? ValueType { get; protected set; }

    /// <summary>
    /// The value of this expression if it is known at compile time
    /// </summary>
    /// <value></value>
    public object? CompileTimeValue { get; protected set; }

    public ValueExpression(ValueExpressionType type, Token token) : base(ExpressionType.Value, token)
    {
        ValueExpressionType = type;
    }
}