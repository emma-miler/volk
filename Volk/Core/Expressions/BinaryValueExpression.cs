using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class BinaryValueExpression : ValueExpression
{
    ValueExpression Left;
    ValueExpression Right;
    OperatorToken Operator;
    
    public BinaryValueExpression(OperatorToken operatorToken, ValueExpression left, ValueExpression right) : base(ValueExpressionType.Binary, operatorToken)
    {
        Left = left;
        Right = right;
        Operator = operatorToken;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[BinaryValueExpression] {Operator}");
        Left.Print(depth + 1);
        Right.Print(depth + 1);
    }

    public override void ResolveNames(Scope scope)
    {
        Left.ResolveNames(scope);
        Right.ResolveNames(scope);
    }
}