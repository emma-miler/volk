using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class ImmediateValueExpression : ValueExpression, IRValue
{
    public string Value => ((ValueToken)Token).Value;
    
    public ImmediateValueExpression(ValueToken token) : base(ValueExpressionType.Immediate, token)
    {
        ValueType = token.ValueType;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ImmediateValueExpression] {Token} '{Value}': '{ValueType}'");
    }

    public override void ResolveNames(Scope scope) {}
}