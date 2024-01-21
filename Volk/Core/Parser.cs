using Osiris;
using Volk.Core.Expressions;

namespace Volk.Core;
public class Parser
{

    public IEnumerable<Scope> Scopes => _scopes;

    Queue<Token> _tokens;
    Stack<Scope> _scopes;

    public Parser(Queue<Token> tokens)
    {
        _tokens = tokens;
        _scopes = new();
        _scopes.Push(new Scope("__root", null!, VKType.BUILTIN_VOID));
    }

    public void Parse()
    {
        while (_tokens.Count != 0)
        {
            ParseStatement();
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
        Token t = PopToken();
        if (t.Type == expected) return t;
        // TODO: proper error message
        Log.Error($"Expected token of type '{expected}', instead got '{t.Type}'");
        Log.Error($"{t}");
        throw new Exception();
    }

    /// <summary>
    /// Dequeues the next token and ensures it is any of the expected types. Throws an exception if it is not
    /// </summary>
    /// <param name="expected"></param>
    /// <returns></returns>
    Token Expect(params TokenType[] expected)
    {
        Token t = PopToken();
        if (expected.Contains(t.Type)) return t;
        // TODO: proper error message
        Log.Error($"Expected token of any type '{string.Join(',', expected)}', instead got '{t.Type}'");
        Log.Error($"{t}");
        throw new Exception();
    }

    /// <summary>
    /// Checks if the next token in the queue is of the expected type. If it is, it dequeues and returns it. If it is not, it returns null.
    /// </summary>
    /// <param name="expected"></param>
    /// <returns>Next token in queue if types match, else null</returns>
    Token? SoftExpect(TokenType expected)
    {
        if (_tokens.Peek().Type == expected) return PopToken();
        else return null;
    }

    Token PopToken() => _tokens.Dequeue();
    Token PeekToken() =>_tokens.Peek();
    Scope ActiveScope() => _scopes.First();

    #endregion Token Functions

    /// <summary>
    /// This function parses a singular top-level value expression. Note: ValueExpressions wrapped in parenthesis are considered a single expression.
    /// </summary>
    /// <returns></returns>
    ValueExpression ConsumeNullaryOrUnaryValueExpression(int depth)
    {
        Log.Trace($"ConsumeNullaryOrUnaryValueExpression with depth {depth}");
        OperatorToken? opToken = null;
        bool isUnaryExpression = false;

        Token t = PeekToken();

        /// ==========
        /// Nested expression
        /// ==========
        // TODO: need to check for function here
        // Maybe need to add a context variable to this function's arguments
        if (t.Type == TokenType.OpenParenthesis)
        {
            Expect(TokenType.OpenParenthesis);
            Log.Trace("consume open paren");
            ValueExpression subExpression = ParseValueExpression(depth + 1, TokenType.CloseParenthesis);
            Expect(TokenType.CloseParenthesis);
            return subExpression;
        }

        /// ==========
        /// Unary Operator
        /// ==========
        else if (t is OperatorToken ot)
        {
            Expect(TokenType.Operator);
            Log.Trace("consume operator");
            opToken = ot;
            if (ot.IsUnaryOperator)
            {
                isUnaryExpression = true;
            }
            else
            {
                Log.Error($"Found non-unary operator '{ot.OperatorType}' at start of value expression");
                // TODO: token->indicate
                throw new Exception();
            }
        }

        PopToken();
        ValueExpression expr;
        if (t is ValueToken vt)
        {
            Log.Trace("consume value token");
            expr = new ImmediateValueExpression(vt);
        }
        else if (t.Type == TokenType.Name)
        {
            Log.Trace("consume name");
            /// ==========
            /// Function call
            /// ==========
            if (_tokens.Peek().Type == TokenType.OpenParenthesis)
            {
                Log.Trace("  consume func call");
                List<ValueExpression> args = new();
                PopToken();
                if (_tokens.Peek().Type == TokenType.CloseParenthesis)
                    PopToken();
                else
                {
                    while (_tokens.Peek().Type != TokenType.CloseParenthesis)
                    {
                        args.Add(ParseValueExpression(depth + 1, TokenType.CommaSeparator));
                        Log.Trace($"func argument {args.Last()}");
                        if (SoftExpect(TokenType.CommaSeparator) == null)
                            break;
                    }
                }
                expr = new FunctionCallValueExpression(t, args);
            }
            /// ==========
            /// LValue reference
            /// ==========
            else
            {
                Log.Trace("  consume lvalue ref");
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
    ValueExpression ParseValueExpression(int depth, TokenType endMarkers)
    {
        bool lastTokenWasOperator = false;
        Stack<ValueExpression> expressions = new();
        Stack<OperatorToken> operators = new();
        Log.Trace($"ParseValueExpression with depth {depth}");
        int i = 0;
        while (true)
        {
            Log.Trace($"Loop idx {depth} {i}");
            i++;
            Token t = PeekToken();
            if (t.Type == endMarkers || (depth > 0 && t.Type == TokenType.CloseParenthesis))
            {
                break;
            }

            // If we encounter an operator token, and the last token was not also an operator,
            // Apply the shunting yard algorithm
            if (t is OperatorToken ot && !lastTokenWasOperator && !ot.IsComparisonOperator)
            {
                Log.Trace("got operator");
                PopToken();
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
                Log.Trace("got comparison operator");
                PopToken();
                operators.Push(comp);
            }
            else
            {
                Log.Trace("getting single value");
                expressions.Push(ConsumeNullaryOrUnaryValueExpression(depth + 1));
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
    void ParseStatement()
    {
        Token t = PeekToken();
        // =========================
        // Close Scope
        // =========================
        if (t.Type == TokenType.CloseCurlyBracket)
        {
            Expect(TokenType.CloseCurlyBracket);
            _scopes.Pop();
            return;
        }

        // =========================
        // Names
        // =========================
        else if (t.Type == TokenType.Name)
        {
            Token peekToken = _tokens.Skip(1).First();
            TokenType peekType = peekToken.Type;
            // =========================
            // Declaration
            // =========================
            if (peekType == TokenType.Name)
            {
                PopToken();
                Token name = Expect(TokenType.Name);
                Token nextToken = Expect(TokenType.Operator, TokenType.EndOfExpression);
                // Declaration + assignment
                if (nextToken.Type == TokenType.Operator && ((OperatorToken)nextToken).OperatorType == OperatorTokenType.Assignment)
                {
                    ActiveScope().Expressions.Add(new DeclarationExpression(t, name));
                    ActiveScope().Expressions.Add(new AssignmentExpression(name, ParseValueExpression(0, TokenType.EndOfExpression)));
                }
                else if (nextToken.Type == TokenType.EndOfExpression)
                {
                    ActiveScope().Expressions.Add(new DeclarationExpression(t, name));
                }
                Expect(TokenType.EndOfExpression);
            }
            // =========================
            // Assignment
            // =========================
            else if (peekToken is OperatorToken ot && ot.OperatorType == OperatorTokenType.Assignment)
            {
                PopToken();
                Expect(TokenType.Operator);
                ActiveScope().Expressions.Add(new AssignmentExpression(t, ParseValueExpression(0, TokenType.EndOfExpression)));
                Expect(TokenType.EndOfExpression);
            }
            // =========================
            // Function call
            // =========================
            else if (peekType == TokenType.OpenParenthesis)
            {
                Log.Trace("parsing function call");
                ActiveScope().Expressions.Add(ParseValueExpression(0, TokenType.CloseParenthesis));
                Expect(TokenType.CloseParenthesis);
                Expect(TokenType.EndOfExpression);
            }
        }

        // =========================
        // If statement
        // =========================
        else if (t.Type == TokenType.If)
        {
            Expect(TokenType.If);
            Expect(TokenType.OpenParenthesis);
            ValueExpression condition = ParseValueExpression(1, TokenType.CloseParenthesis);
            Expect(TokenType.CloseParenthesis);
            Expect(TokenType.OpenCurlyBracket);
            IfExpression expr = new IfExpression(t, condition, ActiveScope());
            ActiveScope().Expressions.Add(expr);
            _scopes.Push(expr.IfTrue);
            return;
        }
        else if (t.Type == TokenType.Else)
        {
            Expect(TokenType.Else);
            Expression lastExpr = ActiveScope().Expressions.Last();
            if (lastExpr is IfExpression ifExpr)
            {
                if (!ifExpr.HasElseClause)
                {
                    ifExpr.HasElseClause = true;
                    Expect(TokenType.OpenCurlyBracket);
                    _scopes.Push(ifExpr.IfFalse);
                    return;
                    
                }
                else
                {
                    Log.Error("Preceding if-statement already has an else clause defined");
                    //TODO: token->indicate
                    throw new Exception();
                }
            }
            else
            {
                Log.Error("Cannot start an else-statement without a preceding if-statement");
                //TODO: token->indicate
                throw new Exception();
            }
        }

        else if (t.Type == TokenType.EOF)
        {
            Log.Info("FINISH PARSE");
            PopToken();
            return;
        }
        else 
        {
            Log.Error($"GRAMMAR ERROR: failed to parse token '{t}'");
            throw new Exception();
        }
    }
}