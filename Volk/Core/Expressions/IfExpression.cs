using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class IfExpression : Expression
{
    ValueExpression Condition;
    public Scope IfTrue;
    public Scope IfFalse;
    public bool HasElseClause { get; set; }

    public IfExpression(Token token, ValueExpression condition, Scope parentScope) : base(ExpressionType.If, token)
    {
        Condition = condition;
        IfTrue = new Scope("__impl_if_true", parentScope, parentScope.ReturnType);
        IfFalse = new Scope("__impl_if_false", parentScope, parentScope.ReturnType);
    }

    public override void Print(int depth)
    {
        string prefix = "|".Repeat(depth);
        Log.Info($"{prefix}[IfExpression]");
        Log.Info($"{prefix}|Condition=");
        Condition.Print(depth + 1);
        Log.Info($"{prefix}|IfTrue=");
        foreach (Expression expr in IfTrue.Expressions)
        {
            expr.Print(depth + 2);
        }
        if (IfFalse != null)
        {
            Log.Info($"{prefix}|IfFalse=");
            foreach (Expression expr in IfFalse.Expressions)
            {
                expr.Print(depth + 2);
            }
        }
    }
}