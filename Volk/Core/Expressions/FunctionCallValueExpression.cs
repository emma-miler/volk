using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class FunctionCallValueExpression : ValueExpression
{
    public List<ValueExpression> Arguments { get; }

    VKObject? _function;


    public FunctionCallValueExpression(Token function, List<ValueExpression> arguments) : base(ValueExpressionType.Call, function)
    {
        Arguments = arguments;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[FunctionCallValueExpression] Func='{Token.Value}'");
        Log.Info($"{prefix} [");
        foreach (ValueExpression expr in Arguments)
        {
            expr.Print(depth + 2);
        }
        Log.Info($"{prefix} ]");
    }

    public override void ResolveNames(Scope scope)
    {
        foreach(ValueExpression expr in Arguments)
        {
            expr.ResolveNames(scope);
        }

        _function = scope.FindVariable(Token.Value);
        if (_function == null)
            throw new NameException($"Undefined function '{Token.Value}'", Token);

        if (_function is VKFunction func)
            ValueType = func.ReturnType;
        else
            throw new TypeException($"Object with name '{Token.Value}' is not a function, as it is of type '{_function.Type}'", Token);
    }
}