using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core;
using Volk.Core.Expressions;

namespace Volk.Parsing
{
    public class ShuntingYard
    {
        public Stack<ValueExpression> Expressions = new();
        public Stack<OperatorToken> Operators = new();

        public void Reduce()
        {
            while (Operators.Count != 0)
            {
                OperatorToken op = Operators.Pop();
                if (op.IsUnaryOperator)
                {
                    if (Expressions.Count < 1) throw new ParseException($"Expected 1 value for unary operator {op.OperatorType}, but got {Expressions.Count}", op);
                    ValueExpression operand = Expressions.Pop();
                    Expressions.Push(new UnaryValueExpression(op, operand));
                }
                else
                {
                    if (Expressions.Count < 2) throw new ParseException($"Expected 2 values for binary operator {op.OperatorType}, but got {Expressions.Count}", op);
                    ValueExpression right = Expressions.Pop();
                    ValueExpression left = Expressions.Pop();
                    if (op.OperatorType == OperatorType.Dot)
                    {
                        if (right.ValueExpressionType != ValueExpressionType.Indirect) throw new ParseException($"Cannot apply operator '{op}' on expression where right-hand side is not a simple name ({right})", op);
                        Expressions.Push(new DotValueExpression(op, left, right.Token));
                    }
                    else if (op.OperatorType == OperatorType.Call)
                    {
                        if (left is IndirectValueExpression indirect)
                            Expressions.Push(new FunctionCallValueExpression(op, (ArgumentPackValueExpression)right, indirect));
                        else if (left is DotValueExpression dotValue)
                            Expressions.Push(new FunctionCallValueExpression(op, (ArgumentPackValueExpression)right, dotValue));
                        else
                            throw new ParseException($"Cannot use expression '{left}' as function reference for function call", left.Token);
                    }
                    else
                    {
                        Expressions.Push(new BinaryValueExpression(op, left, right));
                    }
                }

            }
        }
    }
}