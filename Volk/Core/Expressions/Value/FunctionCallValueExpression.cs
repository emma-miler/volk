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
    ArgumentPackValueExpression _arguments { get; }

    VKFunction? _function;

    string _functionName;
    ValueExpression? _scopeSource;

    public FunctionCallValueExpression(Token token, ArgumentPackValueExpression argumentPack, VKFunction function) : base(ValueExpressionType.Call, token)
    {
        _arguments = argumentPack;
        _functionName = function.Name;
        _function = function;
    }

    public FunctionCallValueExpression(Token function, ArgumentPackValueExpression argumentPack, string functionName, ValueExpression? scopeSource) : base(ValueExpressionType.Call, function)
    {
        _arguments = argumentPack;
        _functionName = functionName;
        _scopeSource = scopeSource;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        if (_function == null)
        {
            if (_scopeSource == null)
                Log.Info($"{prefix}[FunctionCallValueExpression] unresolved '{_functionName}'");
            else
            {
                Log.Info($"{prefix}[FunctionCallValueExpression] unresolved '{_functionName}' with source");
                _scopeSource.Print(depth + 1);
            }
        }
        else
            Log.Info($"{prefix}[FunctionCallValueExpression] {_function}");
        _arguments.Print(depth + 1);
    }

    public override void ResolveNames(VKScope scope)
    {
        _arguments.ResolveNames(scope);

        // If we already have a bound function, no need to look for it
        if (_function != null)
            return;

        // If the scope source turns out to be a type reference, then we may
        // only use static functions, since we are doing
        // Type.Function()
        // as opposed to
        // Instance.Function()
        bool functionMustBeStatic = false;
        if (_scopeSource != null)
        {
            _scopeSource.ResolveNames(scope);
            if (_scopeSource.ValueType == VKType.TYPE)
            {
                functionMustBeStatic = true;
                if (_scopeSource.CompileTimeValue == null)
                    throw new ParseException($"Value of a static type reference must be known at compile, which expression '{_scopeSource}' does not provide.", Token);
                scope = (VKType)_scopeSource.CompileTimeValue;
            }
            else
            {
                scope = _scopeSource.ValueType!;
            }
        }
        else
        {
            functionMustBeStatic = true;
        }

        if (!functionMustBeStatic)
            _arguments.Expressions.Insert(0, _scopeSource!);

        string functionNamePrefix = string.Empty;
        FunctionGroup? possibleFunctions;
        possibleFunctions = scope.FindFunctionGroup(_functionName);
        
        if (possibleFunctions == null) 
            throw new NameException($"No function with name '{functionNamePrefix}{_functionName}' exists", Token);

        List<VKFunction> candidates = possibleFunctions.FindFunction(functionMustBeStatic, _arguments.ArgumentTypes).ToList();

        if (candidates.Count == 0)
        {
            Log.Error($"No variant of function '{functionNamePrefix}{_functionName}' matches argument pack types: {(functionMustBeStatic ? "static " : "")}({string.Join(", ", _arguments.ArgumentTypes)})");
            Log.Error("Possible matches are:");
            foreach (VKFunction function in possibleFunctions.Functions)
            {
                Log.Error(function.ToString());
            }
            throw new NameException($"No variant of function '{functionNamePrefix}{_functionName}' matches argument pack types", Token);
        }
        if (candidates.Count > 1)
        {
            Log.Error($"No variant of function '{functionNamePrefix}{_functionName}' matches argument pack types: {(functionMustBeStatic ? "static " : "")}({string.Join(", ", _arguments.ArgumentTypes)})");
            Log.Error("Possible matches are:");
            foreach (VKFunction function in possibleFunctions.Functions)
            {
                Log.Error(function.ToString());
            }
            throw new NameException($"Ambiguous type resolution for function '{functionNamePrefix}{_functionName}'. {candidates.Count} functions match this argument pack.", Token);
        }

        _function = candidates.Single();
        ValueType = _function.ReturnType;
    }

    public override void TypeCheck(VKScope scope)
    {
        _arguments.TypeCheck(scope);
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        bool hasVarArgs = _function!.Parameters.Any(x => x.Type == VKType.BUILTIN_C_VARARGS);
        gen.Comment("STACK FUNCTION CALL ARGUMENTS");
        List<IRVariable> argVariables = new();
        int i = 0;
        foreach (ValueExpression arg in _arguments.Expressions)
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