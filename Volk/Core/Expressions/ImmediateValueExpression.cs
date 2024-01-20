using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class ImmediateValueExpression : ValueExpression
{
    public string Value => ((ValueToken)Token).Value;
    
    public ImmediateValueExpression(ValueToken token) : base(ValueExpressionType.Immediate, token)
    {
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ImmediateValueExpression]");
        Log.Info($"{prefix} Value={Token}");
    }
}