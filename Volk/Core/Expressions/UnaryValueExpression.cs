using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

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

    public override void ResolveNames(VKScope scope)
    {
        _value.ResolveNames(scope);
    }

    public override void TypeCheck(VKScope scope)
    {
        _value.TypeCheck(scope);
        ValueType = _value.ValueType;
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        IndirectValueExpression? indirectValueExpression = _value as IndirectValueExpression; 
        IRVariable left = _value.GenerateCode(gen);
        gen.Comment("START UNARY OPERATOR");
        if (left.VariableType == IRVariableType.Variable || left.VariableType == IRVariableType.Pointer)
        {
            left = gen.DecayToVariable(left);
            IRVariable ret = gen.NewVariable(left.Type);
            // If the value we are incrementing is a variable, we have to store the new value back in
            gen.Operation($"{ret.Reference} = {_operator.GetIROperator()} nsw i64 {left.Reference}, 1");
            // To do that, we directly get the variable name form the IndirectValueExpression.
            // This will break when the type system gets classes!!!
            if (indirectValueExpression is not null)
            {
                left = new IRVariable(indirectValueExpression.Token.Value, VKType.SYSTEM_POINTER, IRVariableType.Variable);
            }
            gen.Operation($"store {ret}, {left}");
            return ret;
        }
        else
        {
            left = gen.DecayToVariable(left);
            IRVariable ret = gen.NewVariable(left.Type);
            gen.Operation($"{ret.Reference} = {_operator.GetIROperator()} nsw i64 {left.Reference}, 1");
            gen.Comment("END UNARY OPERATOR");
            return ret;
        }
        
    }
}