using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions.Value
{
    public class ConstructorValueExpression : ValueExpression
    {
        Token _typeName;
        ArgumentPackValueExpression _arguments;

        VKType? _targetType;

        VKFunction? _allocator;
        VKFunction? _constructor;

        public ConstructorValueExpression(Token token, Token typename, ArgumentPackValueExpression arguments) : base(ValueExpressionType.Constructor, token)
        {
            _typeName = typename;
            _arguments = arguments;
        }

        public override void Print(int depth)
        {
            string prefix = " ".Repeat(depth);
            if (_targetType != null)
                Log.Info($"{prefix}[ConstructorValueExpression] {_targetType}");
            else
                Log.Info($"{prefix}[ConstructorValueExpression] unresolved '{_typeName}'");
            _arguments.Print(depth + 1);
        }

        public override void ResolveNames(VKScope scope)
        {
            _arguments.ResolveNames(scope);

            _targetType = scope.FindType(_typeName.Value);
            if (_targetType == null)
                throw new ParseException($"Unknown type '{_typeName.Value}'", _typeName);

            List<VKFunction> allocators = _targetType.FindFunction("__allocate", true, new VKType[] {}).ToList();
            if (allocators.Count == 0)
                throw new TypeException($"Type '{_targetType}' has no parameterless allocator functions. Something has gone horribly wrong, as this should never happen!", Token);
            if (allocators.Count > 1)
                throw new TypeException($"Type '{_targetType}' has more than one parameterless allocator function. Something has gone horribly wrong, as this should never happen!", Token);
            _allocator = allocators.Single();

            FunctionGroup? constructorGroup = _targetType.FindFunctionGroup("__new");
            if (constructorGroup == null)
                throw new TypeException($"Type '{_targetType}' has no constructors.", Token);
            List<VKType> constructorArgumentTypes = new() { _targetType };
            constructorArgumentTypes = constructorArgumentTypes.Concat(_arguments.ArgumentTypes).ToList();
            List<VKFunction> constructors = constructorGroup.FindFunction(false, constructorArgumentTypes).ToList();
            if (constructors.Count == 0)
            {
                Log.Error($"Type '{_targetType}' has no constructor matching provided argument types. Argument types were:");   
                Log.Error(string.Join(", ", constructorArgumentTypes));
                Log.Error("Available options are:");
                foreach (VKFunction function in constructors)
                    Log.Error(function.ToString());
                throw new TypeException($"Type '{_targetType}' has no constructor matching constructor.", Token);
            }
            if (constructors.Count > 1)
            {
                Log.Error($"Type '{_targetType}' has more than one possible matching constructor, and one could not be chosen unambiguously. Argument types were:");   
                Log.Error(string.Join(", ", constructorArgumentTypes));
                Log.Error("Ambiguous matches are:");
                foreach (VKFunction function in constructorGroup.Functions)
                    Log.Error(function.ToString());
                throw new TypeException($"Type '{_targetType}' has more than one possible matching constructor.", Token);
            }
            _constructor = constructors.Single();
            ValueType = _targetType;
        }

        public override void TypeCheck(VKScope scope)
        {
            _arguments.TypeCheck(scope);
        }
        
        public override IRVariable GenerateCode(CodeGenerator gen)
        {
            IRVariable allocated = _allocator!.CallInIR(gen);
            List<IRVariable> arguments = new() {allocated};
            foreach (ValueExpression expr in _arguments.Expressions)
            {
                arguments.Add(expr.GenerateCode(gen));
            }
            IRVariable constructed = _constructor!.CallInIR(gen, arguments.ToArray());
            return constructed;
        }
    }
}