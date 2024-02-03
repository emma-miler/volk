using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class ImmediateValueExpression : ValueExpression, IRValue
{
    VKCompileTimeString? _value;
    public string Value => ((ValueToken)Token).Value;
    
    public ImmediateValueExpression(ValueToken token) : base(ValueExpressionType.Immediate, token)
    {
        ValueType = token.ValueType;
    }

    public ImmediateValueExpression(ValueToken token, VKCompileTimeString value) : base(ValueExpressionType.Immediate, token)
    {
        ValueType = token.ValueType;
        _value = value;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ImmediateValueExpression] {Token} '{Value}': '{ValueType}'");
    }

    public override void ResolveNames(Scope scope) {}

    public override void TypeCheck(Scope scope)
    {
        return;
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        if (ValueType == VKType.BUILTIN_STRING)
            return new IRVariable($".str.{_value!.Value.Index}", VKType.BUILTIN_STRING, IRVariableType.Constant);
        else
            return new IRVariable(Value, ValueType!, IRVariableType.Immediate);
    }
}