using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class ClassDeclarationExpression : Expression
{

    VKType _classType;

    public ClassDeclarationExpression(Token token, VKType classType) : base(ExpressionType.ClassDeclaration, token)
    {
        _classType = classType;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ClassDeclarationExpression] {_classType}");
        Log.Info($"{prefix} [");
        foreach (VKField field in _classType.Fields)
        {
            Log.Info(field.ToString());
        }
        Log.Info($"{prefix} ]");
    }

    public override void ResolveNames(VKScope scope)
    {
        foreach (Expression expr in _classType.Expressions)
            expr.ResolveNames(scope);
    }

    public override void TypeCheck(VKScope scope)
    {
        foreach (Expression expr in _classType.Expressions)
            expr.TypeCheck(scope);
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        return new IRVariable(_classType.Name, _classType, IRVariableType.Immediate);
    }
}