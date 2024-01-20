using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Volk.Core.Expressions;

namespace Volk.Core;
public class Parser
{

    public IEnumerable<Expression> Expressions => _expressions;

    Queue<Token> _tokens;
    List<Expression> _expressions;

    public Parser(Queue<Token> tokens)
    {
        _tokens = tokens;
        _expressions = new();
    }

    public void Parse()
    {
        while (_tokens.Count != 0)
        {
            ParseExpression();
        }
    }

    #region Token Functions

    /// <summary>
    /// Dequeues the next token and ensures it is of the expected type. Throws an exception if it is not
    /// </summary>
    /// <param name="expected"></param>
    /// <returns></returns>
    Token Expect(TokenType expected)
    {
        Token t = _tokens.Dequeue();
        if (t.Type == expected) return t;
        // TODO: proper error message
        throw new Exception();
    }

    /// <summary>
    /// Dequeues the next token and ensures it is any of the expected types. Throws an exception if it is not
    /// </summary>
    /// <param name="expected"></param>
    /// <returns></returns>
    Token Expect(params TokenType[] expected)
    {
        Token t = _tokens.Dequeue();
        if (expected.Contains(t.Type)) return t;
        // TODO: proper error message
        throw new Exception();
    }

    /// <summary>
    /// Checks if the next token in the queue is of the expected type. If it is, it dequeues and returns it. If it is not, it returns null.
    /// </summary>
    /// <param name="expected"></param>
    /// <returns>Next token in queue if types match, else null</returns>
    Token? SoftExpect(TokenType expected)
    {
        if (_tokens.Peek().Type == expected) return _tokens.Dequeue();
        else return null;
    }


    #endregion Token Functions

    /// <summary>
    /// This function parses a singular top-level value expression. Note: ValueExpressions wrapped in parenthesis are considered a single expression.
    /// </summary>
    /// <returns></returns>
    ValueExpression ConsumeNullaryOrUnaryValueExpression(Token t)
    {
        OperatorToken? opToken = null;
        bool isUnaryExpression = false;
        /// ==========
        /// Nested expression
        /// ==========
        /// // TODO: need to check for function here
        // Maybe need to add a context variable to this function's arguments
        if (t.Type == TokenType.OpenParenthesis)
        {
            ValueExpression subExpression = ParseValueExpression(TokenType.CloseParenthesis);
            Expect(TokenType.CloseParenthesis);
            return subExpression;
        }

        /// ==========
        /// Unary Operator
        /// ==========
        else if (t is OperatorToken ot)
        {
            opToken = ot;
            if (ot.IsUnaryOperator)
            {
                isUnaryExpression = true;
                t = _tokens.Dequeue();
            }
            else
            {
                Log.Error($"Found non-unary operator '{ot.OperatorType}' at start of value expression");
                // TODO: token->indicate
                throw new Exception();
            }
        }

        ValueExpression expr;
        if (t is ValueToken vt)
        {
            expr = new ImmediateValueExpression(vt);
        }
        else if (t.Type == TokenType.Name)
        {
            // Function call
            if (_tokens.Peek().Type == TokenType.OpenParenthesis)
            {
                List<ValueExpression> args = new();
                _tokens.Dequeue();
                if (_tokens.Peek().Type == TokenType.CloseParenthesis)
                    _tokens.Dequeue();
                else
                {
                    while (_tokens.Peek().Type != TokenType.CloseParenthesis)
                    {
                        args.Add(ParseValueExpression(TokenType.CommaSeparator));
                        if (SoftExpect(TokenType.CommaSeparator) == null)
                        break;
                    }
                }
                expr = new FunctionCallValueExpression(t, args);
            }
            // Variable reference
            else
            {
                expr = new IndirectValueExpression(t);
            }
        }
        else
        {
            Log.Error($"Unexpected token '{t}' while parsing ConsumeNullaryOrUnaryValueExpression");
            // TODO: token->indicate()
            throw new Exception();
        }

        if (!isUnaryExpression)
            return expr;
        else 
            return new UnaryValueExpression(opToken!, expr);
    }

    /// <summary>
    /// This function parses a list of chained values and operators
    /// </summary>
    /// <param name="endMarker"></param>
    /// <returns></returns>
    ValueExpression ParseValueExpression(TokenType endMarker)
    {
        bool lastTokenWasOperator = false;
        Stack<ValueExpression> expressions = new();
        Stack<OperatorToken> operators = new();
        while (true)
        {
            Token t = _tokens.Dequeue();
            if (t.Type == endMarker)
                break;

            // Here we check for singular `<` and `>` tokens since unfortunately they cannot be unambiguously lexed in a context free language
            // So we have to do it here instead
            else if (t.Type == TokenType.CloseAngleBracket)
                t = new OperatorToken(OperatorTokenType.Gt, ">", t.Position.Offset + t.Position.Length, t.Position.Length);
            else if (t.Type == TokenType.OpenAngleBracket)
                t = new OperatorToken(OperatorTokenType.Lt, "<", t.Position.Offset + t.Position.Length, t.Position.Length);

            // If we encounter an operator token, and the last token was not also an operator,
            // Apply the shunting yard algorithm
            if (t is OperatorToken ot && !lastTokenWasOperator && !ot.IsComparisonOperator)
            {
                if (!lastTokenWasOperator)
                {
                    lastTokenWasOperator = true;
                    // If we have only 2 expressions, we don't yet have enough items to complete this chain,
                    // so we push it to the queue
                    if (expressions.Count < 2)
                        operators.Push(ot);
                    // If we have a higher precedence than the last operator, 
                    // we also push it to the queue
                    else if (ot.OperatorType > operators.Last().OperatorType)
                        operators.Push(ot);
                    // Otherwise, collapse the queue and get the value
                    else
                    {
                        // This is an implementation of the shunting yard algorithm
                        while (operators.Count != 0)
                        {
                            OperatorToken op = operators.Pop();
                            ValueExpression left = expressions.Pop();
                            ValueExpression right = expressions.Pop();
                            expressions.Push(new BinaryValueExpression(op, left, right));
                        }
                    }
                    continue;
                }
                else if (ot.OperatorType == OperatorTokenType.Assignment)
                {
                    Log.Error("Cannot assign a value to a variable within a value expression");
                    Log.Error("Did you mean to write '==' instead of '='?");
                    // TODO: token->indicate();
                    throw new Exception();
                }
            }
            else if (t is OperatorToken comp && comp.IsComparisonOperator)
            {
                operators.Push(comp);
            }
            else
            {
                expressions.Push(ConsumeNullaryOrUnaryValueExpression(t));
                lastTokenWasOperator = false;
            }


        }

        // This is an implementation of the shunting yard algorithm
        while (operators.Count != 0)
        {
            OperatorToken op = operators.Pop();
            if (expressions.Count < 2)
            {
                Log.Error($"Expected 2 values for binary operator {op.OperatorType}, but got {expressions.Count}");
                // TODO: token->indicate
                throw new Exception();
            }
            ValueExpression left = expressions.Pop();
            ValueExpression right = expressions.Pop();
            expressions.Push(new BinaryValueExpression(op, left, right));
        }

        return expressions.Single();
    }

    /// <summary>
    /// Parses a single top-level expression. Returns the parsed expression and the number of tokens consumed
    /// </summary>
    /// <param name="tokens"></param>
    /// <returns></returns>
    void ParseExpression()
    {
        Token t = _tokens.Dequeue();
        if (t.Type == TokenType.Name)
        {
            TokenType nextType = _tokens.Peek().Type;
            // =========================
            // Declaration
            // =========================
            if (nextType == TokenType.Name)
            {
                Token name = _tokens.Dequeue();
                Token nextToken = Expect(TokenType.Operator, TokenType.EndOfExpression);
                // Declaration + assignment
                if (nextToken.Type == TokenType.Operator && ((OperatorToken)nextToken).OperatorType == OperatorTokenType.Assignment)
                {
                    _expressions.Add(new DeclarationExpression(t, name));
                    _expressions.Add(new AssignmentExpression(name, ParseValueExpression(TokenType.EndOfExpression)));
                }
                else if (nextToken.Type == TokenType.EndOfExpression)
                {
                    _expressions.Add(new DeclarationExpression(t, name));
                }
            }
        }
    }
}