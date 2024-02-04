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

    public override void ResolveNames(Scope scope)
    {
        Left.ResolveNames(scope);
        Right.ResolveNames(scope);
    }

    public override void TypeCheck(Scope scope)
    {
        Left.TypeCheck(scope);
        Right.TypeCheck(scope);
        ValueType = Left.ValueType;
        if (!VKType.IsEqualOrDerived(Left.ValueType!, Right.ValueType!))
            throw new TypeException($"Cannot apply operator '{Operator}' value of type '{Left.ValueType}' to variable of type '{Right.ValueType}'", Token);
    }

    public override IRVariable GenerateCode(CodeGenerator gen)
    {
        // Generate left-hand side of operator
        gen.Comment("START BINARY OPERATOR VALUE");
        IRVariable left = Left.GenerateCode(gen);
        // Short-circuit logic for logical operators
        if (Operator.OperatorType == OperatorTokenType.LogicalAnd || Operator.OperatorType == OperatorTokenType.LogicalOr)
        {
            string name = Operator.OperatorType == OperatorTokenType.LogicalOr ? "lor" : "land";
            name += gen.Counter++.ToString();

            // Generate the branch instruction
            if (Operator.OperatorType == OperatorTokenType.LogicalAnd) gen.Branch(left, $"{name}.rhs", $"{name}.end");
            if (Operator.OperatorType == OperatorTokenType.LogicalOr) gen.Branch(left, $"{name}.end", $"{name}.rhs");

            // Generate righthand branch
            gen.Label($"{name}.rhs:");
            IRVariable short_right = Right.GenerateCode(gen);
            gen.Jump($"{name}.end:");

            // Generate end of shortcircuit branch
            gen.Label($"{name}.end");
            IRVariable tmp = gen.NewVariable(VKType.BUILTIN_BOOL);

            // Phi node for retrieving value
            if (Operator.OperatorType == OperatorTokenType.LogicalAnd) gen.Operation($"{tmp.Reference} = phi i1 [ false, %{gen.LastJumpPoint} ], [ {short_right.Reference}, {name}.rhs ]");
            if (Operator.OperatorType == OperatorTokenType.LogicalOr) gen.Operation($"{tmp.Reference} = phi i1 [ true, %{gen.LastJumpPoint} ], [ {short_right.Reference}, {name}.rhs ]");

            gen.Comment("END BINARY OPERATOR VALUE");
            return tmp;
        }
        // Generate right-hand side of operator
        IRVariable right = Right.GenerateCode(gen);
        // If either value is a pointer, we first need to dereference it
        if (left.VariableType == IRVariableType.Pointer || right.VariableType == IRVariableType.Pointer)
        {
            gen.Comment("START BINARY OPERATOR DEREFERENCE");
            if (left.VariableType == IRVariableType.Pointer)
            {
                IRVariable tmp = gen.NewVariable(Left.ValueType!);
                // Dereference the variable
                gen.Operation($"{tmp.Reference} = load {left.Type}, {left}");
                left = new IRVariable(tmp.Name, left.Type, IRVariableType.Immediate);
            }
            if (left.VariableType == IRVariableType.Pointer)
            {
                IRVariable tmp = gen.NewVariable(Right.ValueType!);
                // Dereference the variable
                gen.Operation($"{tmp.Reference} = load {right.Type}, {right}");
                right = new IRVariable(tmp.Name, right.Type, IRVariableType.Immediate);
            }
            gen.Comment("END BINARY OPERATOR DEREFERENCE");
        }

        IRVariable ret = gen.NewVariable(Left.ValueType!);
        gen.Comment("START BINARY OPERATOR");
        if (Operator.IsComparisonOperator)
        {
            throw new NotImplementedException();
        }
        else
        {
            string prefix = "";
            // TODO: what?
            if (Left.ValueType == VKType.BUILTIN_REAL)
                prefix = "f";
            if (Operator.OperatorType == OperatorTokenType.Divide || Operator.OperatorType == OperatorTokenType.Modulo)
                prefix = "s";
            gen.Operation($"{ret.Reference} = {prefix}{Operator.GetIROperator()} {left}, {right.Reference}");
        }

        gen.Comment("END BINARY OPERATOR");
        return ret;
    }


    #region Comparison Functions

    #endregion Comparison Functions
}