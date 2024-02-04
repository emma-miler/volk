using Osiris;
using Volk.Core.Expressions;
using Volk.Core.Objects;

namespace Volk.Core;
public class Parser
{

    public IEnumerable<Scope> Scopes => _scopes;

    Queue<Token> _tokens;
    Stack<Scope> _scopes;

    VKProgram _program;

    public Parser(Queue<Token> tokens, VKProgram program)
    {
        _program = program;
        _tokens = tokens;
        _scopes = new();
        Scope root = new Scope("__root", null!, VKType.BUILTIN_INT);
        root.AddObject(VKType.BUILTIN_BOOL);
        root.AddObject(VKType.BUILTIN_REAL);
        root.AddObject(VKType.BUILTIN_INT);
        root.AddObject(VKType.BUILTIN_VOID);
        root.AddObject(VKType.BUILTIN_STRING);
        VKFunction mainFunc = new VKFunction("main", VKType.BUILTIN_INT, new(), null!) {
            Scope = root
        };
        _program.Functions.Add(mainFunc);
        _scopes.Push(root);

        root.AddObject(
            new VKFunction(
                "printf", 
                VKType.BUILTIN_INT, 
                new List<VKObject>() { 
                    new VKObject("format_string", VKType.BUILTIN_STRING),
                    new VKObject("args", VKType.BUILTIN_C_VARARGS),
                },
                root
            )
        );
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
        throw new ParseException($"Expected token of any type '{string.Join(',', expected)}', instead got '{t.Type}'", t);
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
    Token PeekToken() => _tokens.Peek();
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
                throw new ParseException($"Found non-unary operator '{ot.OperatorType}' at start of value expression", ot);
            }
        }

        PopToken();
        ValueExpression expr;
        if (t is ValueToken vt)
        {
            Log.Trace("consume value token");
            if (vt.ValueType == VKType.BUILTIN_STRING)
            {
                VKCompileTimeString cts = new(vt.Value, _program.CompileTimeStrings.Count);
                expr = new ImmediateValueExpression(vt, cts);
                _program.CompileTimeStrings.Add(cts);
            }
            else
            {
                expr = new ImmediateValueExpression(vt);
            }
            
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
                    Expect(TokenType.CloseParenthesis);
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
            throw new ParseException($"Unexpected token '{t}' while parsing ConsumeNullaryOrUnaryValueExpression", t);
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
        Token t;
        while (true)
        {
            Log.Trace($"Loop idx {depth} {i}");
            i++;
            t = PeekToken();
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
                    throw new ParseException("Cannot assign a value to a variable within a value expression. Did you mean to write '==' instead of '='?", ot);
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
            if (op.IsUnaryOperator)
            {
                if (expressions.Count < 1)
                {
                    throw new ParseException($"Expected 1 value for unary operator {op.OperatorType}, but got {expressions.Count}", op);
                }
                ValueExpression operand = expressions.Pop();
                expressions.Push(new UnaryValueExpression(op, operand));
            }
            else
            {
                if (expressions.Count < 2)
                {
                    throw new ParseException($"Expected 2 values for binary operator {op.OperatorType}, but got {expressions.Count}", op);
                }
                ValueExpression left = expressions.Pop();
                ValueExpression right = expressions.Pop();
                expressions.Push(new BinaryValueExpression(op, left, right));
            }
            
        }

        if (!expressions.Any())
        {
            throw new ParseException($"Expected a value expression, but did not find any?", t);
        }

        return expressions.Single();
    }

    /// <summary>
    /// Parses a single top-level expression. Returns the parsed expression and the number of tokens consumed
    /// </summary>
    /// <param name="tokens"></param>
    /// <returns></returns>
    void ParseStatement(TokenType endMarker = TokenType.EndOfExpression)
    {
        Token t = PeekToken();
        // =========================
        // Close Scope
        // =========================
        if (t.Type == TokenType.CloseCurlyBracket)
        {
            Expect(TokenType.CloseCurlyBracket);
            Scope scope = _scopes.Pop();
            scope.CloseScope();
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
                ParseDeclaration(endMarker);

            // =========================
            // Assignment
            // =========================
            else if (peekToken is OperatorToken ot && ot.OperatorType == OperatorTokenType.Assignment)
                ParseAssignment(endMarker);

            // =========================
            // Function call
            // =========================
            else if (peekType == TokenType.OpenParenthesis)
                ParseFunctionCall(endMarker);

            else if (peekType == TokenType.Operator)
            {
                ParseTopLevelValueExpression(endMarker);
            }

            else
            {
                throw new ParseException($"GRAMMAR ERROR: failed to parse token '{peekToken}'", peekToken);
            }
        }

        // =========================
        // If/Else statement
        // =========================
        else if (t.Type == TokenType.If)
            ParseIfStatement(endMarker);
        else if (t.Type == TokenType.Else)
            ParseElseStatement(endMarker);

        // =========================
        // For statement
        // =========================
        else if (t.Type == TokenType.For)
            ParseForStatement(endMarker);

        else if (t.Type == TokenType.FunctionDeclaration)
            ParseFunctionDeclaration(endMarker);

        // =========================
        // For statement
        // =========================
        else if (t.Type == TokenType.Return)
            ParseReturn(endMarker);

        else if (t.Type == TokenType.EOF)
        {
            Log.Info("FINISH PARSE");
            PopToken();
            // Add default return value for main function
            if (ActiveScope().Expressions.Last().ExpressionType != ExpressionType.Return)
            {
                ImmediateValueExpression value = new ImmediateValueExpression(new ValueToken(VKType.BUILTIN_INT, new DummySourcePosition("0")));
                ReturnExpression returnExpr = new ReturnExpression(new Token(TokenType.Return, new DummySourcePosition("return")), value, ActiveScope());
                ActiveScope().Expressions.Add(returnExpr);
            }
            ActiveScope().CloseScope();
            return;
        }
        else
            ParseTopLevelValueExpression(endMarker);
    }

    #region Top Level Statements

    void ParseReturn(TokenType endMarker)
    {
        Token ret = Expect(TokenType.Return);
        Token next = PeekToken();
        if (next.Type == endMarker)
        {
            Expect(endMarker);
            ActiveScope().Expressions.Add(new ReturnExpression(ret, null, ActiveScope()));
        }
        else
        {
            ActiveScope().Expressions.Add(new ReturnExpression(ret, ParseValueExpression(0, endMarker), ActiveScope()));
            Expect(endMarker);
        }
    }

    void ParseFunctionDeclaration(TokenType endMarker)
    {
        Expect(TokenType.FunctionDeclaration);
        Token type = Expect(TokenType.Name);
        Token name = Expect(TokenType.Name);
        VKType? returnType = ActiveScope().FindType(type.Value);
        if  (returnType == null)
                throw new TypeException($"Unknown type '{type}'", type);

        Expect(TokenType.OpenParenthesis);
        // Loop through all arguments
        List<VKObject> parameters = new();
        while (true)
        {
            Token peek = PeekToken();
            // If the next token is a closing parenthesis, we have reached the end of the parameter pack
            if (peek.Type == TokenType.CloseParenthesis)
            {
                Expect(TokenType.CloseParenthesis);
                break;
            }

            // If we haven't reached that point, expect a typename and parameter name
            Token paramType = Expect(TokenType.Name);
            Token paramName = Expect(TokenType.Name);
            // TODO: add support for optional and default parameters right here
            VKType? resolvedType = ActiveScope().FindType(paramType.Value);
            if  (resolvedType == null)
                throw new TypeException($"Unknown type '{paramType}'", paramType);
            parameters.Add(new VKObject(paramName.Value, resolvedType));

            // Then, peek again to see if we've reached the parameter pack end, or if we need to keep parsing
            peek = PeekToken();
            // If the next token is a closing parenthesis, we have reached the end of the parameter pack
            if (peek.Type == TokenType.CloseParenthesis)
            {
                Expect(TokenType.CloseParenthesis);
                break;
            }
            // If we haven't, expect a comma separator
            Expect(TokenType.CommaSeparator);
        }

        VKFunction func = new VKFunction(name.Value, returnType, parameters, ActiveScope());
        ActiveScope().AddObject(func);
        ActiveScope().Expressions.Add(new FunctionDeclarationExpression(name, func));
        _scopes.Push(func.Scope);
        _program.Functions.Add(func);
        Expect(TokenType.OpenCurlyBracket);
    }

    void ParseTopLevelValueExpression(TokenType endMarker)
    {
        // For convenience, we allow value expression to appear as a top level statement
        ActiveScope().Expressions.Add(ParseValueExpression(0, endMarker));
        Expect(endMarker);
    }

    void ParseDeclaration(TokenType endMarker)
    {
        Token t = PopToken();
        Token name = Expect(TokenType.Name);
        Token nextToken = Expect(TokenType.Operator, endMarker);
        VKObject newVar = new VKObject(name.Value, VKType.BUILTIN_ERROR);
        // Declaration + assignment
        if (nextToken.Type == TokenType.Operator && ((OperatorToken)nextToken).OperatorType == OperatorTokenType.Assignment)
        {
            ActiveScope().Expressions.Add(new DeclarationExpression(t, name, newVar));
            ActiveScope().Expressions.Add(new AssignmentExpression(name, ParseValueExpression(0, endMarker)));
            Expect(endMarker);
        }
        // Only declaration
        else if (nextToken.Type == endMarker)
        {
            ActiveScope().Expressions.Add(new DeclarationExpression(t, name, newVar));
        }
        
        ActiveScope().AddObject(newVar);
    }

    void ParseAssignment(TokenType endMarker)
    {
        Token t = PopToken();
        Expect(TokenType.Operator);
        ActiveScope().Expressions.Add(new AssignmentExpression(t, ParseValueExpression(0, endMarker)));
        Expect(endMarker);
    }

    void ParseFunctionCall(TokenType endMarker)
    {
        Log.Trace("parsing function call");
        ActiveScope().Expressions.Add(ParseValueExpression(0, endMarker));
        Expect(endMarker);
    }

    void ParseIfStatement(TokenType endMarker)

    {
        Token t = Expect(TokenType.If);
        Expect(TokenType.OpenParenthesis);
        ValueExpression condition = ParseValueExpression(1, TokenType.CloseParenthesis);
        Expect(TokenType.CloseParenthesis);
        Expect(TokenType.OpenCurlyBracket);
        IfExpression expr = new IfExpression(t, condition, ActiveScope());
        ActiveScope().Expressions.Add(expr);
        _scopes.Push(expr.IfTrue);
        return;
    }

    void ParseElseStatement(TokenType endMarker)
    {
        Token t = Expect(TokenType.Else);
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
                throw new ParseException("Preceding if-statement already has an else clause defined", t);
            }
        }
        else
        {
            throw new ParseException("Cannot start an else-statement without a preceding if-statement", t);
        }
    }

    void ParseForStatement(TokenType endMarker)
    {
        Token t = Expect(TokenType.For);

        Scope parentScope = ActiveScope();
        Expect(TokenType.OpenParenthesis);
        
        // Create a new scope that will be shared by the for loop expressions and the for loop body
        Scope scope = new Scope("__impl_for", parentScope, parentScope.ReturnType);
        _scopes.Push(scope);

        // Parse a singular gramatically top level statement
        // Note: because a compound declaration and assignment such as 'int i = 0' is modelled as two top level statements,
        // we can actually have multiple statements here. I don't really want to introduce a single declare-and-assign expression,
        // so this will have to do for now.
        ParseStatement();
        List<Expression> initializers = scope.Expressions.ToList();
        scope.Expressions.Clear();

        // Then, parse the loop conditional
        ValueExpression conditional = ParseValueExpression(0, TokenType.EndOfExpression);
        Expect(TokenType.EndOfExpression);

        // Then, parse the action statement
        ParseStatement(TokenType.CloseParenthesis);
        Expression action = scope.Expressions.Single();
        scope.Expressions.Clear();

        Expect(TokenType.OpenCurlyBracket);

        ForExpression forExpr = new ForExpression(t, initializers, conditional, action, scope);
        parentScope.Expressions.Add(forExpr);
    }

    #endregion Top Level Statements
}