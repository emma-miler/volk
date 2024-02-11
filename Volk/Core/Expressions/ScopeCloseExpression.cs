using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class ScopeCloseExpression : Expression
{
    public ScopeCloseExpression(Token token) : base(ExpressionType.ScopeClose, token) {}

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        return new IRVariable("__scope_close", VKType.SYSTEM_ERROR, IRVariableType.Immediate);
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}ScopeCloseExpression");
    }

    public override void ResolveNames(VKScope scope) {}
    public override void TypeCheck(VKScope scope) {}
}