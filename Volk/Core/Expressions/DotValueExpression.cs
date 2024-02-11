using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class DotValueExpression : ValueExpression
{

    ValueExpression _left;
    Token _right;

    VKField? _field;

    public DotValueExpression(Token token, ValueExpression left, Token right) : base(ValueExpressionType.Dot, token)
    {
        _left = left;
        _right = right;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[DotValueExpression] {_right}");
        _left.Print(depth + 1);
    }


    public override void ResolveNames(VKScope scope)
    {
        _left.ResolveNames(scope);
        VKObject? obj = _left.ValueType!.FindVariable(_right.Value);
        if (obj == null)
            throw new NameException($"Unknown field '{_right.Value}' on type '{_left.ValueType!}'", Token);
        _field = (VKField)obj;
        ValueType = _field.Type;
    }

    public override void TypeCheck(VKScope scope)
    {
        _left.TypeCheck(scope);
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        IRVariable ret = gen.NewVariable(_field!.Type, IRVariableType.Pointer);
        IRVariable left = _left.GenerateCode(gen);
        gen.Operation($"{ret.Reference} = getelementptr inbounds %class.{_left.ValueType!.Name}, ptr {left.Reference}, i64 {_field.Offset}");
        return ret;
    }

   
    
}