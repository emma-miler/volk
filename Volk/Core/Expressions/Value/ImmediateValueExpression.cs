using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Expressions.Internal;
using Volk.Core.Expressions.Value;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class ImmediateValueExpression : ValueExpression
{
    VKCompileTimeString? _strValue;
    public string Value => ((ValueToken)Token).Value;

    ValueExpression? _valueExpression;
    
    public ImmediateValueExpression(ValueToken token) : base(ValueExpressionType.Immediate, token)
    {
        ValueType = token.ValueType;
    }

    public ImmediateValueExpression(ValueToken token, VKCompileTimeString value) : base(ValueExpressionType.Immediate, token)
    {
        _strValue = value;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ImmediateValueExpression] {Token} '{Value}': '{ValueType}'");
    }

    public override void ResolveNames(VKScope scope) 
    {
        if (_strValue != null)
        {
            ValueType = VKType.STRING;
            ArgumentPackValueExpression args = new ArgumentPackValueExpression(Token, new List<ValueExpression>()
            {
                new ConstantValueExpression(Token, VKType.SYSTEM_C_STRING, $".str.{_strValue!.Index}"),
                new ImmediateValueExpression(new ValueToken(VKType.INT, new DummySourcePosition(_strValue.Value.Length.ToString())))
            });
            _valueExpression = new ConstructorValueExpression(Token, ValueType!, args);
            _valueExpression.ResolveNames(scope);
        }
    }

    public override void TypeCheck(VKScope scope) 
    {
        if (_valueExpression != null)
            _valueExpression.TypeCheck(scope);
    }
    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        if (_strValue != null)
            return _valueExpression!.GenerateCode(gen);
        else if (ValueType == VKType.REAL)
            return new IRVariable("0x" + BitConverter.ToString(BitConverter.GetBytes(double.Parse(Value)).Reverse().ToArray()).Replace("-", ""), ValueType!, IRVariableType.Immediate);
        else
            return new IRVariable(Value, ValueType!, IRVariableType.Immediate);
    }
}