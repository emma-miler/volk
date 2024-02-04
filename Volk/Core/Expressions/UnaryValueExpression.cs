using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions;
public class UnaryValueExpression : ValueExpression
{
    ValueExpression _value;
    OperatorToken _operator;
    
    public UnaryValueExpression(OperatorToken operatorToken, ValueExpression value) : base(ValueExpressionType.Unary, operatorToken)
    {
        _value = value;
        _operator = operatorToken;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[UnaryValueExpression] {_operator.Value}");
        _value.Print(depth + 1);
    }

    public override void ResolveNames(Scope scope)
    {
        _value.ResolveNames(scope);
    }

    public override void TypeCheck(Scope scope)
    {
        _value.TypeCheck(scope);
        ValueType = _value.ValueType;
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        IRVariable left = _value.GenerateCode(gen);
        gen.Comment("START UNARY OPERATOR");

        if (left.VariableType == IRVariableType.Pointer)
        {
            IRVariable tmp = gen.NewVariable(left.Type, IRVariableType.Immediate);
            gen.Comment("!!! CHECK CODE 5101");
            gen.Operation($"{tmp.Reference} = load i64, ptr %{left}");
            left = tmp;
        }
        IRVariable ret = gen.NewVariable(left.Type, IRVariableType.Immediate);
        gen.Operation($"{ret.Reference} = {_operator.GetIROperator()} nsw i64 0, {left}");
        gen.Comment("END UNARY OPERATOR");
        return ret;
    }
}