using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class FunctionCallValueExpression : ValueExpression
{
    public List<ValueExpression> Arguments { get; }


    public FunctionCallValueExpression(Token function, List<ValueExpression> arguments) : base(ValueExpressionType.Call, function)
    {
        Arguments = arguments;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[FunctionCallValueExpression]");
        Log.Info($"{prefix} Func={Token.Value}");
        Log.Info($"{prefix} Args=");
        foreach (ValueExpression expr in Arguments)
        {
            expr.Print(depth + 1);
        }
    }
}