using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class ImplicitCastExpression : ValueExpression
{

    ValueExpression _value;
    VKType _targetType;
    VKFunction? _castFunction;

    public ImplicitCastExpression(Token token, ValueExpression valueExpression, VKType targetType) : base(ValueExpressionType.ValueCast, token)
    {
        _value = valueExpression;
        _targetType = targetType;
    }


    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ImplicitCastExpression] {Token.Value}");
        _value?.Print(depth + 1);
    }

    public override void ResolveNames(VKScope scope)
    {
        // NOTE: probably redundant since this expression only gets created after typechecking anyway
        //_value.ResolveNames(scope);
    }

    public override void TypeCheck(VKScope scope)
    {
        // NOTE: probably redundant since this expression only gets created after typechecking anyway
        //_value.TypeCheck(scope);

        _castFunction = _value.ValueType!.FindCastFunction(_targetType);
        if (_castFunction == null)
            throw new TypeException($"Cannot implicitly cast from type '{_value.ValueType}' to type '{_targetType}'", Token);
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        IRVariable var = _value.GenerateCode(gen);
        return _castFunction!.CallInIR(gen, var);
    }
}