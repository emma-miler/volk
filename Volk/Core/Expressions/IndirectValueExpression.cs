using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class IndirectValueExpression : ValueExpression, ILValue
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

    public override void ResolveNames(Scope scope)
    {
        _variable = scope.FindVariable(Token.Value);
        if (_variable == null)
            throw new NameException($"Undefined variable '{Token.Value}'", Token);
        ValueType = _variable.Type;
    }

    public override void TypeCheck(Scope scope)
    {
        return;
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        // If it is already the most recent value on the stack, we don't need to do anything
        IRVariable ret = gen.NewVariable(_variable!.Type);
        // Assign the value
        gen.Operation($"{ret.Reference} = load {_variable.Type.IRType}, ptr %{_variable.Name}");
        return ret;
    }
}