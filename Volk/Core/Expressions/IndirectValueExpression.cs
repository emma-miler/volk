using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class IndirectValueExpression : ValueExpression
{
    
    public IndirectValueExpression(Token name) : base(ValueExpressionType.Indirect, name)
    {
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[IndirectValueExpression]");
        Log.Info($"{prefix} Name={Token.Value}");
    }
}