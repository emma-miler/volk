using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions;
public class BinaryValueExpression : ValueExpression
{
    ValueExpression Left;
    ValueExpression Right;
    OperatorToken Operator;

    VKFunction? _function;

    public BinaryValueExpression(OperatorToken operatorToken, ValueExpression left, ValueExpression right) : base(ValueExpressionType.Binary, operatorToken)
    {
        Left = left;
        Right = right;
        Operator = operatorToken;
    }

    public override void Print(int depth)
    {
        string prefix = " ".Repeat(depth);
        Log.Info($"{prefix}[BinaryValueExpression] {Operator}: {Left.ValueType}");
        Left.Print(depth + 1);
        Right.Print(depth + 1);
    }

    public override void ResolveNames(VKScope scope)
    {
        Left.ResolveNames(scope);
        Right.ResolveNames(scope);
    }

    public override void TypeCheck(VKScope scope)
    {
        Left.TypeCheck(scope);
        Right.TypeCheck(scope);
        if (!VKType.IsEqualOrDerived(Left.ValueType!, Right.ValueType!))
            throw new TypeException($"Cannot apply operator '{Operator}' value of type '{Left.ValueType}' to variable of type '{Right.ValueType}'", Token);

        // TODO: this might be broken, oops
        _function = Left.ValueType!.FindFunction($"__{Operator.OperatorType}", false, new List<VKType> { Right.ValueType!, Left.ValueType }).FirstOrDefault();
        if (_function == null)
            throw new TypeException($"Type '{Left.ValueType}' does not support operation '{Operator.OperatorType}' with type '{Right.ValueType}'", Operator);
        ValueType = _function.ReturnType;
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        // Generate left-hand side of operator
        gen.Comment("START BINARY OPERATOR VALUE");
        IRVariable left = Left.GenerateCode(gen);
        // Short-circuit logic for logical operators
        if (Operator.OperatorType == OperatorType.LogicalAnd || Operator.OperatorType == OperatorType.LogicalOr)
        {
            string name = Operator.OperatorType == OperatorType.LogicalOr ? "lor" : "land";
            name += gen.Counter.ToString();

            // Generate the branch instruction
            if (Operator.OperatorType == OperatorType.LogicalAnd) gen.Branch(left, $"{name}.rhs", $"{name}.end");
            if (Operator.OperatorType == OperatorType.LogicalOr) gen.Branch(left, $"{name}.end", $"{name}.rhs");

            // Generate righthand branch
            gen.Label($"{name}.rhs:");
            IRVariable short_right = Right.GenerateCode(gen);
            gen.Jump($"{name}.end:");

            // Generate end of shortcircuit branch
            gen.Label($"{name}.end");
            IRVariable tmp = gen.NewVariable(VKType.BOOL);

            // Phi node for retrieving value
            if (Operator.OperatorType == OperatorType.LogicalAnd) gen.Operation($"{tmp.Reference} = phi i1 [ false, %{gen.LastJumpPoint} ], [ {short_right.Reference}, {name}.rhs ]");
            if (Operator.OperatorType == OperatorType.LogicalOr) gen.Operation($"{tmp.Reference} = phi i1 [ true, %{gen.LastJumpPoint} ], [ {short_right.Reference}, {name}.rhs ]");

            gen.Comment("END BINARY OPERATOR VALUE");
            return tmp;
        }
        // Generate right-hand side of operator
        IRVariable right = Right.GenerateCode(gen);
        // If either value is a pointer, we first need to dereference it
        if (left.PointerDepth > 0 || right.PointerDepth > 0)
        {
            gen.Comment("START BINARY OPERATOR DEREFERENCE");
            left = gen.DecayToVariable(left);
            right = gen.DecayToVariable(right);
            gen.Comment("END BINARY OPERATOR DEREFERENCE");
        }

        IRVariable ret;
        gen.Comment("START BINARY OPERATOR");
        ret = _function!.CallInIR(gen, left, right);
        gen.Comment("END BINARY OPERATOR");
        return ret;
    }


    #region Comparison Functions

    #endregion Comparison Functions
}