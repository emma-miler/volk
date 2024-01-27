using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class ForExpression : Expression
{
    public List<Expression> Initializer;
    public ValueExpression Condition;
    public Expression Action;

    public Scope Scope;

    public bool HasElseClause { get; set; }

    public ForExpression(Token token, List<Expression> initializer, ValueExpression condition, Expression action, Scope scope) : base(ExpressionType.For, token)
    {
        Initializer = initializer;
        Condition = condition;
        Action = action;
        Scope = scope;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ForExpression]");
        Log.Info($"{prefix} Initializer=");
        foreach (Expression init in Initializer)
        {
            init.Print(depth + 2);
        }
        Log.Info($"{prefix} Condition=");
        Condition.Print(depth + 1);
        Log.Info($"{prefix} Action=");
        Action.Print(depth + 1);
        Log.Info($"{prefix} Scope=");
        foreach (Expression expr in Scope.Expressions)
        {
            expr.Print(depth + 2);
        }
    }
}