using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class CastValueExpression : ValueExpression
{

    ValueExpression _value;
    VKType? _targetType;
    VKFunction? _castFunction;
    ValueExpression? _targetTypeExpression;

    public CastValueExpression(Token token, ValueExpression valueExpression, VKType targetType) : base(ValueExpressionType.ValueCast, token)
    {
        _value = valueExpression;
        _targetType = targetType;
    }

    public CastValueExpression(Token token, ValueExpression valueExpression, ValueExpression targetTypeExpression) : base(ValueExpressionType.ValueCast, token)
    {
        _value = valueExpression;
        _targetTypeExpression = targetTypeExpression;
    }


    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ImplicitCastExpression] {Token.Value}");
        _value?.Print(depth + 1);
    }

    public override void ResolveNames(VKScope scope)
    {
        _value.ResolveNames(scope);
        if (_targetType == null)
        {
            _targetTypeExpression!.ResolveNames(scope);
            if (_targetTypeExpression.CompileTimeValue == null)
                throw new ParseException($"Value of a static type reference must be known at compile, which expression '{_targetTypeExpression}' does not provide.", Token);
            _targetType = (VKType)_targetTypeExpression.CompileTimeValue;
        }

        _castFunction = _targetType.FindCastFunction(_value.ValueType!);
        if (_castFunction == null)
            throw new TypeException($"Cannot cast from type '{_value.ValueType}' to type '{_targetType}'", Token);
    }

    public override void TypeCheck(VKScope scope) {}

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        IRVariable var = _value.GenerateCode(gen);
        return _castFunction!.CallInIR(gen, var);
    }
}