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

    public VKObject? Value {get; private set; }

    public IndirectValueExpression(Token name) : base(ValueExpressionType.Indirect, name)
    {
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        if (Value == null)
            Log.Info($"{prefix}[IndirectValueExpression] '{Token.Value}'");
        else
            Log.Info($"{prefix}[IndirectValueExpression] '{Value.Name}': {Value.Type}");
    }

    public override void ResolveNames(VKScope scope)
    {
        Value = scope.FindVariable(Token.Value);
        if (Value != null)
        {
            ValueType = Value.Type;
        }
        else
        {
            Value = scope.FindType(Token.Value);
            if (Value == null)
            {
                Log.Debug($"Scope: {scope.ChainName}");
                throw new NameException($"Undefined variable '{Token.Value}'", Token);
            }
            CompileTimeValue = Value;
            ValueType = VKType.TYPE;
        }
    }

    public override void TypeCheck(VKScope scope)
    {
        return;
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        return new IRVariable(Value!.Name, Value!.Type, IRVariableType.Variable, 1);
    }
}