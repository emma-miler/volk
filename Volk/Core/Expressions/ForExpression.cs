using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class ForExpression : Expression
{
    public List<Expression> Initializer;
    public ValueExpression Condition;
    public Expression Increment;

    public Scope Scope;

    public bool HasElseClause { get; set; }

    public ForExpression(Token token, List<Expression> initializer, ValueExpression condition, Expression increment, Scope scope) : base(ExpressionType.For, token)
    {
        Initializer = initializer;
        Condition = condition;
        Increment = increment;
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
        Increment.Print(depth + 1);
        Log.Info($"{prefix} Scope=");
        foreach (Expression expr in Scope.Expressions)
        {
            expr.Print(depth + 2);
        }
    }

    public override void ResolveNames(Scope scope)
    {
        foreach (Expression expr in Initializer)
        {
            expr.ResolveNames(Scope);
        }
        Condition.ResolveNames(Scope);
        Increment.ResolveNames(Scope);
        foreach (Expression expr in Scope.Expressions)
        {
            expr.ResolveNames(Scope);
        }
    }

    public override void TypeCheck(Scope scope)
    {
        foreach (Expression initializer in Initializer)
        {
            initializer.TypeCheck(scope);
        }
        Condition.TypeCheck(scope);
        Increment.TypeCheck(scope);

        foreach (Expression expr in Scope.Expressions)
        {
            expr.TypeCheck(Scope);
        }
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        string name = "for" + gen.Counter++.ToString();
        gen.Comment("START FOR_LOOP INITITALIZER");
        foreach (Expression initializer in Initializer)
        {
            initializer.GenerateCode(gen);
        }
        gen.Comment("END FOR_LOOP INITITALIZER");

        gen.Comment("START FOR_LOOP CONDITION");
        gen.Jump($"{name}.condition");
        gen.Label($"{name}.condition:");
        // Generate IR for conditional
        IRVariable conditionVar = Condition.GenerateCode(gen);
        // Derefence the value if its a pointer value
        conditionVar = gen.DereferenceIfPointer(conditionVar);
        // If it it's for example int, truncate it to bool
        if (conditionVar.Type != VKType.BUILTIN_BOOL)
        {
            IRVariable tmp = gen.NewVariable(VKType.BUILTIN_BOOL, IRVariableType.Immediate);
            gen.Operation($"{tmp} = trunc {conditionVar} to i1");
            conditionVar = tmp;
        }
        gen.Branch(conditionVar, $"{name}.body", $"{name}.end");
        gen.Comment("END FOR_LOOP CONDITION");
        // TODO: is this important?
        gen.LastJumpPoint = $"{name}.body";

        gen.Comment("START FOR_LOOP BODY");
        // Generate for expression body
        foreach (Expression expr in Scope.Expressions)
        {
            expr.GenerateCode(gen);
        }
        gen.Comment("END FOR_LOOP BODY");

        gen.Comment("START FOR_LOOP INCREMENT");
        // Generate loop increment
        IRVariable inc = Increment.GenerateCode(gen);
        gen.Comment("END FOR_LOOP INCREMENT");
        // Jump back to loop start
        gen.Jump($"{name}.condition");
        
        gen.Label($"{name}.end");
        return inc;
    }
}