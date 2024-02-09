using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

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
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[IfExpression]");
        Log.Info($"{prefix} {{Condition}}");
        Condition.Print(depth + 1);
        Log.Info($"{prefix} {{IfTrue}}");
        foreach (Expression expr in IfTrue.Expressions)
        {
            expr.Print(depth + 1);
        }
        if (IfFalse != null)
        {
            Log.Info($"{prefix} {{IfFalse}}");
            foreach (Expression expr in IfFalse.Expressions)
            {
                expr.Print(depth + 1);
            }
        }
    }

    public override void ResolveNames(Scope scope)
    {
        Condition.ResolveNames(scope);
        foreach (Expression expr in IfTrue.Expressions)
        {
            expr.ResolveNames(scope);
        }
        foreach (Expression expr in IfFalse.Expressions)
        {
            expr.ResolveNames(scope);
        }
    }

    public override void TypeCheck(Scope scope)
    {
        Condition.TypeCheck(scope);

        if (Condition.ValueType != VKType.BUILTIN_BOOL)
        {
            Condition = new ImplicitCastExpression(Token, Condition, VKType.BUILTIN_BOOL);
            Condition.TypeCheck(scope);
        }
        

        foreach (Expression expr in IfTrue.Expressions)
        {
            expr.TypeCheck(IfTrue);
        }
        foreach (Expression expr in IfFalse.Expressions)
        {
            expr.TypeCheck(IfFalse);
        }
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        gen.Comment("START IF CONDITION");
        IRVariable condition = Condition.GenerateCode(gen);
        // Derefence the value if its a pointer value
        condition = gen.DereferenceIfPointer(condition);
        gen.Comment("END IF CONDITION");

        string name = "if" + gen.Counter.ToString();
        if (HasElseClause)
            gen.Branch(condition, $"{name}.then", $"{name}.else");
        else
            gen.Branch(condition, $"{name}.then", $"{name}.end");

        gen.Comment("START IF BODY");
        gen.Label($"{name}.then:");
        foreach (Expression expr in IfTrue.Expressions)
            expr.GenerateCode(gen);
        gen.Jump($"{name}.end");
        gen.Comment("END IF BODY");

        if (HasElseClause)
        {
            gen.Comment("START ELSE BODY");
            gen.Label($"{name}.else:");
            foreach (Expression expr in IfFalse.Expressions)
                expr.GenerateCode(gen);
            gen.Jump($"{name}.end");
            gen.Comment("END ELSE BODY");
        }

        gen.Label($"{name}.end:");
        if (HasElseClause)
            gen.Operation("unreachable");

        return condition;
    }
}