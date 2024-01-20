using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class AssignmentExpression : Expression
{
    ValueExpression Value;
    
    public AssignmentExpression(Token name, ValueExpression value) : base(ExpressionType.Assigment, name)
    {
        Value = value;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[AssignmentExpression]");
        Log.Info($"{prefix} Name={Token.Value}");
        Log.Info($"{prefix} Value=");
        Value.Print(depth + 1);
    }
}