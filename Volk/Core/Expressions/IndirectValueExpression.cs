using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class IndirectValueExpression : ValueExpression
{

    VKObject? _variable;
    
    public IndirectValueExpression(Token name) : base(ValueExpressionType.Indirect, name)
    {
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        if (_variable == null)
            Log.Info($"{prefix}[IndirectValueExpression] '{Token.Value}'");
        else
            Log.Info($"{prefix}[IndirectValueExpression] '{_variable.Name}': {_variable.Type}");
    }

    public override void ResolveNames(VKScope scope)
    {
        _variable = scope.FindVariable(Token.Value);
        if (_variable == null)
            throw new NameException($"Undefined variable '{Token.Value}'", Token);
        ValueType = _variable.Type;
    }

    public override void TypeCheck(VKScope scope)
    {
        return;
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        return new IRVariable(_variable!.Name, _variable!.Type, IRVariableType.Variable, 1);
    }
}