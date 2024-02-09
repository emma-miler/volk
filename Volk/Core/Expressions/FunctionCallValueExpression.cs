using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class FunctionCallValueExpression : ValueExpression
{
    public List<ValueExpression> Arguments { get; }

    VKFunction? _function;


    public FunctionCallValueExpression(Token function, List<ValueExpression> arguments) : base(ValueExpressionType.Call, function)
    {
        Arguments = arguments;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        if (_function == null)
            Log.Info($"{prefix}[FunctionCallValueExpression] unresolved '{Token.Value}'");
        else
            Log.Info($"{prefix}[FunctionCallValueExpression] {_function}");
        Log.Info($"{prefix} [");
        foreach (ValueExpression expr in Arguments)
        {
            expr.Print(depth + 2);
        }
        Log.Info($"{prefix} ]");
    }

    public override void ResolveNames(Scope scope)
    {
        foreach(ValueExpression expr in Arguments)
        {
            expr.ResolveNames(scope);
        }

        _function = (VKFunction?)scope.FindVariable(Token.Value) ?? throw new NameException($"Undefined function '{Token.Value}'", Token);

        if (_function is VKFunction func)
            ValueType = func.ReturnType;
        else
            throw new TypeException($"Object with name '{Token.Value}' is not a function, as it is of type '{_function.Type}'", Token);
    }

    public override void TypeCheck(Scope scope)
    {
        int i = 0;
        foreach (ValueExpression arg in Arguments)
        {
            if (_function!.Parameters[i].Type == VKType.BUILTIN_C_VARARGS)
            {
                // If we hit a varargs, it doesnt make sense to continue searching, cause we cant guarantee type safety anymore anyway
                return;
            }
            arg.TypeCheck(scope);
            VKObject parameter = _function!.Parameters[i];
            if (parameter.Type != arg.ValueType)
                throw new TypeException($"Cannot use value of type '{arg.ValueType}' in parameter slot for type '{parameter.Type}'", Token);
            i++;
        }
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        bool hasVarArgs = _function!.Parameters.Any(x => x.Type == VKType.BUILTIN_C_VARARGS);
        gen.Comment("STACK FUNCTION CALL ARGUMENTS");
        List<IRVariable> argVariables = new();
        int i = 0;
        foreach (ValueExpression arg in Arguments)
        {
            gen.Comment($"ARG {i}");
            IRVariable argVar = arg.GenerateCode(gen);
            argVar = gen.DereferenceIfPointer(argVar);
            Log.Debug($"Has varargs: {hasVarArgs}");
            Log.Debug($"Arg type: {arg.ValueType}");
            argVariables.Add(argVar);
            i++;
        }
        gen.Comment("END FUNCTION CALL ARGUMENTS");
        gen.Comment("START FUNCTION CALL");
        IRVariable retVal = _function.CallInIR(gen, argVariables.ToArray());
        gen.Comment("END FUNCTION CALL");
        return retVal;
    }
}