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

    public ValueExpression Expression { get; }
    Token _right;
    public string Name { get; }

    VKField? _field;

    public DotValueExpression(Token token, ValueExpression left, Token right) : base(ValueExpressionType.Dot, token)
    {
        Expression = left;
        Name = right.Value;
        _right = right;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[DotValueExpression] {_right} '{Name}'");
        Expression.Print(depth + 1);
    }


    public override void ResolveNames(VKScope scope)
    {
        Expression.ResolveNames(scope);
        VKObject? obj = Expression.ValueType!.FindVariable(Name);
        if (obj == null)
            throw new NameException($"Unknown field '{Name}' on type '{Expression.ValueType!}'", Token);
        _field = (VKField)obj;
        ValueType = _field.Type;
    }

    public override void TypeCheck(VKScope scope)
    {
        Expression.TypeCheck(scope);
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        IRVariable left = Expression.GenerateCode(gen);
        left = gen.DecayToVariable(left);
        IRVariable ret = gen.NewPointerVariable(_field!.Type);
        gen.Operation($"{ret.Reference} = getelementptr inbounds %class.{Expression.ValueType!.Name}, ptr {left.Reference}, i32 0, i32 {_field.Offset}");
        return ret;
    }



}