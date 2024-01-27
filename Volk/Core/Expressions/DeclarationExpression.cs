using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class DeclarationExpression : Expression
{
    Token Typename;

    VKObject _variable;

    public DeclarationExpression(Token type, Token name, VKObject variable) : base(ExpressionType.Declaration, name)
    {
        Typename = type;
        _variable = variable;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[DeclarationExpression] '{_variable.Name}': {_variable.Type}");
    }

    public override void ResolveNames(Scope scope)
    {
        VKType? varType = scope.FindType(Typename.Value);
        if (varType == null)
            throw new NameException($"Declared variable '{_variable.Name}' of unknown type '{Typename.Value}'", Token);
        _variable.UpdateType(varType);
    }
}