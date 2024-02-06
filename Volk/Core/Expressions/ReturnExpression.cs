using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class ReturnExpression : Expression
{
    ValueExpression? _value;
    Scope _scope;
    
    public ReturnExpression(Token token, ValueExpression? value, Scope currentScope) : base(ExpressionType.Return, token)
    {
        _value = value;
        _scope = currentScope;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[ReturnExpression] {Token.Value}");
        _value?.Print(depth + 1);
    }

    public override void ResolveNames(Scope scope)
    {
        _value?.ResolveNames(scope);
    }

    public override void TypeCheck(Scope scope)
    {
        if (_value != null)
        {
            _value.TypeCheck(scope);
            if (!VKType.IsEqualOrDerived(_value.ValueType!, _scope.ReturnType))
                throw new TypeException($"Cannot return value of type '{_value.ValueType}' from scope of type '{_scope.ReturnType}'", Token);
        }
        else
        {
            if (_scope.ReturnType != VKType.BUILTIN_VOID)
                throw new TypeException($"Cannot return value of type 'void' from scope of type '{_scope.ReturnType}'", Token);
        }
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        IRVariable ret;
        if (_value != null)
        {
            if (_value.ValueType == VKType.BUILTIN_VOID)
            {
                gen.Counter++;
                gen.Operation($"ret void");
                return new IRVariable("__void", VKType.BUILTIN_VOID, IRVariableType.Immediate);
            }
            gen.Comment("START RETURN VALUE");
            ret = _value.GenerateCode(gen);
            ret = gen.DereferenceIfPointer(ret);
            gen.Operation($"ret {ret}");
            gen.Comment("END RETURN VALUE");
        }
        else
        {
            ret = new IRVariable("__err", VKType.BUILTIN_VOID, IRVariableType.Immediate);
            gen.Operation("ret void");
        }
        return ret;
    }
}