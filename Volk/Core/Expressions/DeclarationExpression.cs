using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class DeclarationExpression : Expression
{
    Token Typename;

    public DeclarationExpression(Token type, Token name) : base(ExpressionType.Declaration, name)
    {
        Typename = type;
    }

    public override void Print(int depth)
    {
        string prefix = "|".Repeat(depth);
        Log.Info($"{prefix}[DeclarationExpression]");
        Log.Info($"{prefix}|Type={Typename.Value}");
        Log.Info($"{prefix}|Name={Token.Value}");
    }
}