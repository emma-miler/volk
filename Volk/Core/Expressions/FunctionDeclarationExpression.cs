using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class FunctionDeclarationExpression : Expression
{
    VKFunction _function;

    public FunctionDeclarationExpression(Token token, VKFunction function) : base(ExpressionType.Function, token)
    {
        _function = function;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[FunctionDeclarationExpression] Func='{_function.Name}'");
    }

    public override void ResolveNames(Scope scope)
    {
        foreach(Expression expr in _function.Scope.Expressions)
        {
            expr.ResolveNames(_function.Scope);
        }
    }

    public override void TypeCheck(Scope scope)
    {
        return;
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        return new IRVariable(_function.Name, VKType.BUILTIN_FUNCTION, IRVariableType.Immediate);
    }
}