using System.Linq.Expressions;
using System.Security.Cryptography.X509Certificates;
using Osiris;
using Volk.Core.Expressions;
using Volk.Core.Expressions.Internal;
using Volk.Core.Objects;
using Volk.Parsing;

namespace Volk.Core;
public class Parser
{

    public IEnumerable<VKScope> Scopes => _scopes;

    Queue<Token> _tokens;
    Stack<VKScope> _scopes = new();

    VKProgram _program;

    public Parser(Queue<Token> tokens, VKProgram program)
    {
        _program = program;
        _tokens = tokens;
        _scopes.Push(program.RootScope);
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
        throw new ParseException($"Expected token of type '{expected}', instead got '{t.Type}'", t);
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
    VKScope ActiveScope() => _scopes.First();

    #endregion Token Functions

    List<ValueExpression> ParseFunctionCall(int depth)
    {
        Log.Trace("  consume func call");
        List<ValueExpression> args = new();
        Expect(TokenType.OpenParenthesis);
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
        return args;
    }

    /// <summary>
    /// This function parses a singular top-level value expression. Note: ValueExpressions wrapped in parenthesis are considered a single expression.
    /// </summary>
    /// <returns></returns>
    ValueExpression ConsumeNullaryOrUnaryValueExpression(int depth, bool expectFunctionCall = false)
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
            if (expectFunctionCall)
            {
                return new ArgumentPackValueExpression(t, ParseFunctionCall(depth));
            }
            else
            {
                Expect(TokenType.OpenParenthesis);
                Log.Trace("consume open paren");
                ValueExpression subExpression = ParseValueExpression(depth + 1, TokenType.CloseParenthesis);
                Token next = PeekToken();
                Expect(TokenType.CloseParenthesis);
                return subExpression;
            }
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
            if (vt.ValueType == VKType.STRING)
            {
                VKCompileTimeString cts = _program.AddCompileTimeString(vt.Value);
                expr = new ImmediateValueExpression(vt, cts);
            }
            else
            {
                expr = new ImmediateValueExpression(vt);
            }
            
        }
        else if (t.Type == TokenType.Name)
        {
            Log.Trace("  consume lvalue ref");
            expr = new IndirectValueExpression(t);
        }
        /// ==========
        /// Type instantiation
        /// ==========
        else if (t.Type == TokenType.NewExpression)
        {
            Log.Trace("consume new expression");
            Token nameToken = Expect(TokenType.Name);
            Expect(TokenType.OpenParenthesis);
            List<ValueExpression> args = new();
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
            IndirectValueExpression typeName = new IndirectValueExpression(nameToken);
            DotValueExpression constructorName = new DotValueExpression(t, typeName, new Token(TokenType.Name, new DummySourcePosition("__new")));
            expr = new FunctionCallValueExpression(t, new ArgumentPackValueExpression(t, args), constructorName);
        }
        else
            throw new ParseException($"Unexpected token '{t}' while parsing ConsumeNullaryOrUnaryValueExpression", t);

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
    ValueExpression ParseValueExpression(int depth, TokenType endMarkers, bool breakOnAssignment = false)
    {
        bool lastTokenWasOperator = false;
        ShuntingYard yard = new();
        Log.Trace($"ParseValueExpression with depth {depth}");
        int i = 0;
        Token t;
        while (true)
        {
            Log.Trace($"Loop idx {depth} {i}");
            i++;
            t = PeekToken();
            if (t.Type == endMarkers || (depth > 0 && t.Type == TokenType.CloseParenthesis) || (breakOnAssignment && t.Type == TokenType.Operator && ((OperatorToken)t).OperatorType == OperatorType.Assignment))
            {
                break;
            }

            // If we encounter an operator token, and the last token was not also an operator,
            // Apply the shunting yard algorithm
            if (t is OperatorToken ot && !ot.IsComparisonOperator&& !lastTokenWasOperator)
            {
                PopToken();
                Log.Trace("got operator");
                if (!lastTokenWasOperator)
                {
                    lastTokenWasOperator = true;
                    // If we have only 2 expressions, we don't yet have enough items to complete this chain,
                    // so we push it to the queue
                    if (yard.Expressions.Count < 2)
                        yard.Operators.Push(ot);
                    // If we have a higher precedence than the last operator, 
                    // we also push it to the queue
                    else if (yard.Operators.Count() == 0 || ot.OperatorType > yard.Operators.Last().OperatorType)
                        yard.Operators.Push(ot);
                    // Otherwise, collapse the queue and get the value
                    else
                    {
                        yard.Reduce();
                    }
                    continue;
                }
                else if (ot.OperatorType == OperatorType.Assignment)
                {
                    throw new ParseException("Cannot assign a value to a variable within a value expression. Did you mean to write '==' instead of '='?", ot);
                }
            }
            else if (t is OperatorToken comp && comp.IsComparisonOperator)
            {
                Log.Trace("got comparison operator");
                PopToken();
                yard.Operators.Push(comp);
            }
            else
            {
                Log.Trace("getting single value");
                if (t.Type == TokenType.Name)
                {
                    PopToken();
                    yard.Expressions.Push(new IndirectValueExpression(t));
                    if (PeekToken().Type == TokenType.OpenParenthesis)
                    {
                        yard.Reduce();
                        yard.Operators.Push(new OperatorToken(OperatorType.Call, PeekToken().ValueSource));
                        yard.Expressions.Push(ConsumeNullaryOrUnaryValueExpression(depth + 1, true));
                    }
                }
                else
                {
                    yard.Expressions.Push(ConsumeNullaryOrUnaryValueExpression(depth + 1, false));
                }
                
                lastTokenWasOperator = false;
            }
        }

        // This is an implementation of the shunting yard algorithm
        yard.Reduce();

        int exprCount = yard.Expressions.Count();
        if (exprCount == 0)
            throw new ParseException($"Expected a value expression, but did not find any", t);
        if (exprCount == 2)
            throw new ParseException($"Expected a single value expression, but found multiple. This is a syntax error", t);

        return yard.Expressions.Single();
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
        // Commentstatement
        // =========================
        if (t.Type == TokenType.Comment)
            PopToken();

        // =========================
        // Close Scope
        // =========================
        else if (t.Type == TokenType.CloseCurlyBracket)
        {
            Expect(TokenType.CloseCurlyBracket);
            VKScope scope = _scopes.Pop();
            scope.Expressions.Add(new ScopeCloseExpression(t));
            scope.CloseScope();
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
            else if (peekToken is OperatorToken ot && ot.OperatorType == OperatorType.Assignment)
                ParseSimpleAssignment(endMarker);

            // =========================
            // Function call
            // =========================
            else if (peekType == TokenType.OpenParenthesis)
                ParseFunctionCall(endMarker);

            else if (peekType == TokenType.Operator)
            {
                ParseTopLevelValueExpressionOrAssignment(endMarker, true);
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
            ParseIfStatement();
        else if (t.Type == TokenType.Else)
            ParseElseStatement();

        // =========================
        // For statement
        // =========================
        else if (t.Type == TokenType.For)
            ParseForStatement();

        // =========================
        // Function Declaration
        // =========================
        else if (t.Type == TokenType.FunctionDeclaration)
        {
            VKFunction func = ParseFunctionDeclaration();
            ActiveScope().AddFunction(func);
            ActiveScope().Expressions.Add(new FunctionDeclarationExpression(func.Token!, func));
            _scopes.Push(func.Scope);
            _program.Functions.Add(func);
            Expect(TokenType.OpenCurlyBracket);
        }

        // =========================
        // Class Declaration
        // =========================
        else if (t.Type == TokenType.ClassDeclaration)
            ParseClassDeclaration();

        // =========================
        // Return statement
        // =========================
        else if (t.Type == TokenType.Return)
            ParseReturn(endMarker);

        else if (t.Type == TokenType.EOF)
        {
            Log.Info("FINISH PARSE");
            PopToken();

            if (ActiveScope().Expressions.LastOrDefault()?.ExpressionType != ExpressionType.Return)
            {
                ImmediateValueExpression value = new ImmediateValueExpression(new ValueToken(VKType.INT, new DummySourcePosition("0")));
                ReturnExpression returnExpr = new ReturnExpression(new Token(TokenType.Return, new DummySourcePosition("return")), value, ActiveScope());
                ActiveScope().Expressions.Add(returnExpr);
            }

            ActiveScope().CloseScope();
        }
        else
            ParseTopLevelValueExpressionOrAssignment(endMarker);
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

    void ParseClassDeclaration()
    {
        Token decl = Expect(TokenType.ClassDeclaration);
        Token name = Expect(TokenType.Name);
        Expect(TokenType.OpenCurlyBracket);
        VKType type = new VKType(name.Value, true, ActiveScope());
        _scopes.Push(type);
        while (PeekToken().Type != TokenType.CloseCurlyBracket)
        {
            Token t = PeekToken();
            if (t.Type == TokenType.Name)
                ParseDeclaration(TokenType.EndOfExpression);
            else if (t.Type == TokenType.KeywordConstructor)
                ParseConstructorDeclaration(type);
            else if (t.Type == TokenType.FunctionDeclaration)
            {
                VKFunction func = ParseFunctionDeclaration(false);
                VKObject thisParam = new VKObject("this", type);
                func.Parameters.Insert(0, thisParam);
                func.Scope.AddObject(thisParam);

                ActiveScope().AddFunction(func);
                ActiveScope().Expressions.Add(new FunctionDeclarationExpression(func.Token!, func));
                _scopes.Push(func.Scope);
                _program.Functions.Add(func);
                Expect(TokenType.OpenCurlyBracket);
                while (PeekToken().Type != TokenType.CloseCurlyBracket)
                {
                    ParseStatement();
                }
                Expect(TokenType.CloseCurlyBracket);
                _scopes.Pop();
            }
            else
                throw new ParseException($"Unexpected token '{t}' during class declaration", t);
        }
        Expect(TokenType.CloseCurlyBracket);
        _scopes.Pop();
        ActiveScope().AddType(type);
        ActiveScope().Expressions.Add(new ClassDeclarationExpression(decl, type));
    }

    VKFunction ParseConstructorDeclaration(VKType type)
    {
        Token token = Expect(TokenType.KeywordConstructor);
        
        List<VKObject> parameters = new();
        parameters.AddRange(ParseFunctionHead());
        
        VKFunction func = new VKFunction(ActiveScope(), "__new", type, true, parameters.ToArray());
        Token thisToken = new Token(TokenType.Name, new DummySourcePosition("this"));
        Token typeToken = new Token(TokenType.Name, new DummySourcePosition(type.Name));
        VKObject thisObject = new VKObject("this", type);
        InstanceAllocationExpression alloc = new InstanceAllocationExpression(token, type);
        DeclarationExpression decl = new DeclarationExpression(typeToken, thisToken, thisObject);
        AssignmentExpression assignmentExpression = new AssignmentExpression(token, new IndirectValueExpression(thisToken), alloc);
        func.Scope.AddObject(thisObject);
        func.Scope.Expressions.Add(decl);
        func.Scope.Expressions.Add(assignmentExpression);
        type.AddFunction(func);
        ActiveScope().Expressions.Add(new FunctionDeclarationExpression(token, func));
        _scopes.Push(func.Scope);
        _program.Functions.Add(func);
        Expect(TokenType.OpenCurlyBracket);
        while (PeekToken().Type != TokenType.CloseCurlyBracket)
        {
            ParseStatement();
        }
        Expect(TokenType.CloseCurlyBracket);

        IndirectValueExpression returnThisExpr = new IndirectValueExpression(thisToken);
        ReturnExpression returnExpr = new ReturnExpression(token, returnThisExpr, func.Scope);
        func.Scope.Expressions.Add(returnExpr);
        _scopes.Pop();
        return func;
    }

    VKFunction ParseFunctionDeclaration(bool isStatic = true)
    {
        Expect(TokenType.FunctionDeclaration);
        Token type = Expect(TokenType.Name);
        Token name = Expect(TokenType.Name);
        VKType? returnType = ActiveScope().FindType(type.Value);
        if (returnType == null)
            throw new TypeException($"Unknown type '{type}'", type);

        List<VKObject> parameters = ParseFunctionHead();
        
        return new VKFunction(ActiveScope(), name.Value, returnType, isStatic, parameters.ToArray())
        {
            Token = name
        };
    }


    List<VKObject> ParseFunctionHead()
    {
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
                throw new TypeException($"Unknown type '{paramType.Value}'", paramType);
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
        return parameters;
    }

    void ParseTopLevelValueExpressionOrAssignment(TokenType endMarker, bool breakOnAssignment = false)
    {
        // For convenience, we allow value expression to appear as a top level statement
        ValueExpression left = ParseValueExpression(0, endMarker, breakOnAssignment);
        if (PeekToken().Type == TokenType.Operator)
        {
            OperatorToken assignment = (OperatorToken)Expect(TokenType.Operator);
            if (assignment.OperatorType != OperatorType.Assignment)
                throw new ParseException($"Expected assignment operator, instead got '{assignment}'. Something went very wrong!", assignment);
            ValueExpression right = ParseValueExpression(0, endMarker, breakOnAssignment);
            ActiveScope().Expressions.Add(new AssignmentExpression(assignment, left, right));
            Expect(endMarker);
        }
        else
        {
            ActiveScope().Expressions.Add(left);
            Expect(endMarker);
        }
    }

    void ParseDeclaration(TokenType endMarker)
    {
        Token t = PopToken();
        Token name = Expect(TokenType.Name);
        Token nextToken = Expect(TokenType.Operator, endMarker);
        VKObject newVar = new VKObject(name.Value, VKType.SYSTEM_ERROR);
        newVar = ActiveScope().AddObject(newVar);
        // Declaration + assignment
        if (nextToken.Type == TokenType.Operator && ((OperatorToken)nextToken).OperatorType == OperatorType.Assignment)
        {
            ActiveScope().Expressions.Add(new DeclarationExpression(t, name, newVar));
            ActiveScope().Expressions.Add(new AssignmentExpression(nextToken, new IndirectValueExpression(name), ParseValueExpression(0, endMarker)));
            Expect(endMarker);
        }
        // Only declaration
        else if (nextToken.Type == endMarker)
        {
            ActiveScope().Expressions.Add(new DeclarationExpression(t, name, newVar));
        }
        
        
    }

    void ParseSimpleAssignment(TokenType endMarker)
    {
        Token t = PopToken();
        OperatorToken assign = (OperatorToken)Expect(TokenType.Operator);
        if (assign.OperatorType != OperatorType.Assignment)
            throw new ParseException($"Operator '{assign}' is not valid in this position", assign);
        ActiveScope().Expressions.Add(new AssignmentExpression(assign, new IndirectValueExpression(t), ParseValueExpression(0, endMarker)));
        Expect(endMarker);
    }

    void ParseFunctionCall(TokenType endMarker)
    {
        Log.Trace("parsing function call");
        ActiveScope().Expressions.Add(ParseValueExpression(0, endMarker));
        Expect(endMarker);
    }

    void ParseIfStatement()

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

    void ParseElseStatement()
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

    void ParseForStatement()
    {
        Token t = Expect(TokenType.For);

        VKScope parentScope = ActiveScope();
        Expect(TokenType.OpenParenthesis);
        
        // Create a new scope that will be shared by the for loop expressions and the for loop body
        VKScope scope = new VKScope("__impl_for", parentScope, parentScope.ReturnType);
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