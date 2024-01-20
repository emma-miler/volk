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
    bool IsComparison;
    
    public UnaryValueExpression(OperatorToken operatorToken, ValueExpression value) : base(ValueExpressionType.Unary, operatorToken)
    {
        Value = value;
        Operator = operatorToken;
        IsComparison = false;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ImmediateValueExpression]");
        Log.Info($"{prefix} Operator={Operator.Value}");
        Log.Info($"{prefix} Value=");
        Value.Print(depth + 1);
    }
}