using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class UnaryValueExpression : ValueExpression
{
    ValueExpression Value;
    OperatorToken Operator;
    
    public UnaryValueExpression(OperatorToken operatorToken, ValueExpression value) : base(ValueExpressionType.Unary, operatorToken)
    {
        Value = value;
        Operator = operatorToken;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[UnaryValueExpression] {Operator.Value}");
        Value.Print(depth + 1);
    }

    public override void ResolveNames(Scope scope)
    {
        Value.ResolveNames(scope);
    }
}