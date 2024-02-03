using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class AssignmentExpression : Expression
{
    ValueExpression _valueExpression;
    
    VKObject? _variable;

    public AssignmentExpression(Token name, ValueExpression value) : base(ExpressionType.Assigment, name)
    {
        _valueExpression = value;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[AssignmentExpression] '{Token.Value}': {_variable?.Type.Name ?? "__builtin_error"}");
        _valueExpression.Print(depth + 1);
    }

    public override void ResolveNames(Scope scope)
    {
        _variable = scope.FindVariable(Token.Value);
        if (_variable == null)
            throw new NameException($"Could not find bound object with name '{Token.Value}'", Token);
        _valueExpression.ResolveNames(scope);
    }

    public override void TypeCheck(Scope scope)
    {
        _valueExpression.TypeCheck(scope);
        if (!VKType.IsEqualOrDerived(_variable!.Type, _valueExpression.ValueType!))
            throw new TypeException($"Cannot assign value of type '{_valueExpression.ValueType}' to variable of type '{_variable!.Type}'", Token);
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        // We check if we have an immediate value here for optimization
        if (_valueExpression.ValueExpressionType != ValueExpressionType.Immediate)
            gen.Comment("START ASSIGNMENT VALUE");
        IRVariable value = _valueExpression.GenerateCode(gen);
        gen.Comment("START ASSIGNMENT");
        gen.Operation($"store {value}, ptr %{_variable!.Name}");
        gen.Comment("END ASSIGNMENT");
        return new IRVariable(_variable.Name, _variable.Type, IRVariableType.Variable);
    }
}