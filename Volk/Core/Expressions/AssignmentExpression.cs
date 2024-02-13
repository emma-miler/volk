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
    ValueExpression _left;
    ValueExpression _right;

    public AssignmentExpression(Token name, ValueExpression left, ValueExpression right) : base(ExpressionType.Assigment, name)
    {
        _left = left;
        _right = right;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[AssignmentExpression] '{Token.Value}': {_left.ValueType?.Name ?? "__builtin_error"}");
        _left!.Print(depth + 1);
        _right!.Print(depth + 1);
    }

    public override void ResolveNames(VKScope scope)
    {
        _left.ResolveNames(scope);
        _right.ResolveNames(scope);
    }

    public override void TypeCheck(VKScope scope)
    {
        _left.TypeCheck(scope);
        _right.TypeCheck(scope);
        if (!VKType.IsEqualOrDerived(_left.ValueType!, _right.ValueType!))
            throw new TypeException($"Cannot assign value of type '{_right.ValueType}' to variable of type '{_left!.ValueType}'", Token);
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        // We check if we have an immediate value here for optimization
        if (_right.ValueExpressionType != ValueExpressionType.Immediate)
            gen.Comment("START ASSIGNMENT VALUE");
        IRVariable leftVar = _left.GenerateCode(gen);
        IRVariable rightVar = _right.GenerateCode(gen);
        gen.Comment("START ASSIGNMENT");
        gen.Operation($"store {rightVar}, ptr {leftVar.Reference}");
        gen.Comment("END ASSIGNMENT");
        return rightVar;
    }
}