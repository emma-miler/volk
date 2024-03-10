using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class FunctionCallValueExpression : ValueExpression
{
    public ArgumentPackValueExpression ArgumentPack { get; }

    VKFunction? _function;

    string _functionName;
    ValueExpression? _classInstance;

    public FunctionCallValueExpression(Token token, ArgumentPackValueExpression argumentPack, VKFunction function) : base(ValueExpressionType.Call, token)
    {
        ArgumentPack = argumentPack;
        _functionName = function.Name;
        _function = function;
    }

    public FunctionCallValueExpression(Token function, ArgumentPackValueExpression argumentPack, IndirectValueExpression functionNameReference) : base(ValueExpressionType.Call, function)
    {
        ArgumentPack = argumentPack;
        _functionName = functionNameReference.Token.Value;
    }

    public FunctionCallValueExpression(Token function, ArgumentPackValueExpression argumentPack, DotValueExpression dotValueExpression) : base(ValueExpressionType.Call, function)
    {
        ArgumentPack = argumentPack;
        _functionName = dotValueExpression.Name;
        _classInstance = dotValueExpression.Expression;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        if (_function == null)
        {
            if (_classInstance == null)
                Log.Info($"{prefix}[FunctionCallValueExpression] unresolved '{_functionName}'");
            else
            {
                Log.Info($"{prefix}[FunctionCallValueExpression] unresolved '{_functionName}' with DotExpression");
                _classInstance.Print(depth + 1);
            }
        }
        else
            Log.Info($"{prefix}[FunctionCallValueExpression] {_function}");
        ArgumentPack.Print(depth + 1);
    }

    public override void ResolveNames(VKScope scope)
    {
        foreach (ValueExpression expr in ArgumentPack.Expressions)
        {
            expr.ResolveNames(scope);
        }

        // If we already have a bound function, no need to look for it
        if (_function != null)
            return;

        string functionNamePrefix = string.Empty;

        if (_classInstance != null)
        {
            _classInstance.ResolveNames(scope);
            _function = _classInstance.ValueType!.FindFunction(_functionName, ArgumentPack.ArgumentTypes);
            functionNamePrefix = _classInstance.ValueType.Name + "::";
        }
        else
        {
            _function = scope.FindFunction(_functionName, ArgumentPack.ArgumentTypes);
        }

        if (_function == null)
            throw new NameException($"Undefined function '{functionNamePrefix}{_functionName}'", Token);

        ValueType = _function.ReturnType;

        if (!_function.IsStatic)
        {
            ArgumentPack.Expressions.Insert(0, _classInstance!);
        }
    }

    public override void TypeCheck(VKScope scope)
    {
        int i = 0;
        foreach (ValueExpression arg in ArgumentPack.Expressions)
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
        foreach (ValueExpression arg in ArgumentPack.Expressions)
        {
            gen.Comment($"ARG {i}");
            IRVariable argVar = arg.GenerateCode(gen);
            argVar = gen.DecayToVariable(argVar);
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