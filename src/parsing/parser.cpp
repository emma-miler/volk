#include "parser.h"
#include "../log/log.h"
#include "../core/keyword.h"
#include "../core/operator.h"
#include "../core/exceptions.h"
#include "../util/string.h"
#include "../core/token/string.h"
#include "../core/token/operator.h"
#include "../core/exceptions.h"

#include <memory>
#include <optional>

#include <spdlog/spdlog.h>


namespace Volk
{

void VKParser::popToken()
{
    Program->Tokens.pop_front();
}


std::shared_ptr<Token> VKParser::expectToken(TokenType expectedType)
{
    std::shared_ptr<Token> token = Program->Tokens.front(); popToken();
    if (token->Type != expectedType)
    {
        Log::PARSER->error("{}", Program->Source->Lines[token->Position.LineIndex]);
        std::string space = fmt::format("{: >{}}", ' ', token->Position.LineOffset);
        std::string tokenIndicator = fmt::format("{:^>{}}", ' ', token->Position.Length + 1);
        Log::PARSER->error(space + tokenIndicator);
        Log::PARSER->error("Expected token type '{}', instead found '{}'", TokenTypeNames[expectedType], TokenTypeNames[token->Type]);
        Log::PARSER->error("Managed to parse the following tokens:");
        Program->printExpressionTree();

        throw parse_error("");
    }
    return token;
}

std::optional<std::shared_ptr<Token>> VKParser::softExpectToken(TokenType expectedType)
{
    std::shared_ptr<Token> token = Program->Tokens.front(); popToken();
    if (token->Type != expectedType)
    {
        return std::nullopt;
    }
    return token;
}


std::shared_ptr<ValueExpression> VKParser::ConsumeNullaryOrUnaryValueExpression(int depth)
{
    OperatorType opType = OperatorType::Null;
	bool isUnaryExpression = false;

    /// ==========
    /// Nested expression
    /// ==========
    // TODO: need to check for function here
    // Maybe need to add a context variable to this function's arguments
    if (Program->Tokens.front()->Type == TokenType::OpenParenthesis)
    {
        expectToken(TokenType::OpenParenthesis);
        auto temp = parseValueExpression(depth + 1, TokenType::CloseParenthesis);
        expectToken(TokenType::CloseParenthesis);
        return temp;
    }

    /// ==========
    /// Unary Operator
    /// ==========
    if (Program->Tokens.front()->Type == TokenType::Operator)
    {
        OperatorToken operatorToken = *static_cast<OperatorToken*>(Program->Tokens.front().get()); 
		popToken();
        // Check if operator is unary
        auto unaryOperator = std::find(UnaryOperators.begin(), UnaryOperators.end(), operatorToken.OpType);
        Log::PARSER->trace("UNARY: {}", operatorToken.ToString());
        if (unaryOperator == UnaryOperators.end())
        {
            Log::PARSER->critical("Found non-unary operator '{}' at start of value expression", operatorToken.ToString());
            throw parse_error("");
        }
        opType = *unaryOperator;
		isUnaryExpression = true;
    }

    std::shared_ptr<Token> token = Program->Tokens.front(); 
	popToken();
    std::shared_ptr<ValueExpression> expr;
    if (token->Type == TokenType::ImmediateIntValue)
    {
        expr = std::make_shared<ImmediateValueExpression>(std::static_pointer_cast<ValueToken>(token));
        expr->ResolvedType = Program->DefaultScope->FindType("int");
    }
    else if (token->Type == TokenType::ImmediateFloatValue)
    {
        auto temp = std::make_shared<ImmediateValueExpression>(std::static_pointer_cast<ValueToken>(token));
        double tempValue = (double)(float)atof(temp->Value.c_str());
        temp->Value = fmt::format("{}", *(void**)(&tempValue));
        expr = std::move(temp);
        expr->ResolvedType = BUILTIN_FLOAT;
    }
    else if (token->Type == TokenType::ImmediateDoubleValue)
    {
        expr = std::make_shared<ImmediateValueExpression>(std::static_pointer_cast<ValueToken>(token));
        expr->ResolvedType = BUILTIN_DOUBLE;
    }
    else if (token->Type == TokenType::ImmediateBoolValue)
    {
        expr = std::make_shared<ImmediateValueExpression>(std::static_pointer_cast<ValueToken>(token));
        expr->ResolvedType = BUILTIN_BOOL;
    }
    else if (token->Type == TokenType::Name)
    {
        // Function call
        if (Program->Tokens.front()->Type == TokenType::OpenParenthesis)
        {
            std::shared_ptr<FunctionCallValueExpression> func = std::make_shared<FunctionCallValueExpression>(token->Value, token);
            popToken();
            int i = 0;
            if (Program->Tokens.front()->Type == TokenType::CloseParenthesis)
            {
                expectToken(TokenType::CloseParenthesis);
            }
            else
            {
                while (Program->Tokens.front()->Type != TokenType::CloseParenthesis)
                {
                    func->Arguments.push_back(parseValueExpression(depth + 1, TokenType::CommaSeperator));
                    Log::PARSER->trace("func argument {}", i);
                    i++;
                    if (!softExpectToken(TokenType::CommaSeperator).has_value())
                        break;
                }
            }
            expr = func;
        }
        else
        {
            expr = std::make_unique<IndirectValueExpression>(token);
        }
    }
    else if (token->Type == TokenType::StringConstant)
    {
        expr = std::make_unique<StringConstantValueExpression>(token);
        expr->ResolvedType = Program->DefaultScope->FindType("string");
    }
    else
    {
        Log::PARSER->critical("Unexpected token {} in while parsing ConsumeNullaryOrUnaryValueExpression", token->ToString());
        token->Indicate();
        throw parse_error("");
    }

    if (!isUnaryExpression)
    {
        return expr;
    }
    else    
	{
        return std::make_unique<UnaryValueExpression>(opType, std::move(expr), token);
    }
}

std::shared_ptr<ValueExpression> VKParser::parseValueExpression(int depth, TokenType endMarker)
{
    Log::PARSER->trace("parseValueExpression depth = {}", depth);
    std::shared_ptr<OperatorToken> operatorToken = OperatorToken::Dummy();
	std::deque<std::shared_ptr<ValueExpression>> expressions;
	std::deque<std::shared_ptr<OperatorToken>> operators;

	bool lastTokenWasOperator = true;

	int i = -1;
	// We use the shunting-yard algorithm to do operator precedence
    while (Program->Tokens.size() != 0)
    {
		i++;
		Log::PARSER->trace("Loop index        : {}", i);
		Log::PARSER->trace("front token = {}", Program->Tokens.front()->ToString());
        // If we are the top level of a nested expression, we look for an EndOfStatement token
        // Otherwise, we look for a CloseExpressionScope token, since we must first close all
        // the open ExpressionScopes, aka every '(' must have a matching ')'
        if ((depth == 0 && Program->Tokens.front()->Type == TokenType::EndOfStatement) ||
            (depth > 0 && Program->Tokens.front()->Type == TokenType::CloseParenthesis) ||
			(Program->Tokens.front()->Type == endMarker))
        {
            break;
        }

        TokenType frontType = Program->Tokens.front()->Type;
		
		
		// If last token was already an operator, it's likely that this operator token is a unary one
		// If it isnt, we error on that somewhere else
        if (frontType == TokenType::Operator && !lastTokenWasOperator)
		{
			lastTokenWasOperator = true;
			operatorToken = dynamic_pointer_cast<OperatorToken>(Program->Tokens.front()); popToken();
			// Not enough items to compute this value yet
			if (expressions.size() < 2)
				operators.push_back(operatorToken);
			// Or if we have higher precedence
			else if (operatorToken->OpType > operators.back()->OpType)
				operators.push_back(operatorToken);
			// Otherwise, calculate
			else
			{
				while (!operators.empty())
				{
					std::shared_ptr<OperatorToken> op = operators.back();
					operators.pop_back();
					std::shared_ptr<ValueExpression> right = expressions.back();
					expressions.pop_back();
					std::shared_ptr<ValueExpression> left = expressions.back();
					expressions.pop_back();
					expressions.push_back(std::make_unique<BinaryValueExpression>(op->OpType, std::move(left), std::move(right), op));
				}
				operators.push_back(operatorToken);
			}
			continue;
		}
		else if (frontType == TokenType::EqualSign ||
                 frontType == TokenType::CloseAngleBrace ||
                 frontType == TokenType::OpenAngleBrace ||
                 frontType == TokenType::ExclamationMark)
        {
            lastTokenWasOperator = true;
            std::shared_ptr<OperatorToken> opToken = std::make_shared<OperatorToken>("+", Program->Tokens.front()->Position); popToken();
            opToken->IsComparator = true;
            if (frontType == TokenType::EqualSign)
            {
                std::shared_ptr<Token> frontToken = Program->Tokens.front();
                if (Program->Tokens.front()->Type != TokenType::EqualSign)
                {
                    Log::PARSER->error("Cannot assign a value to a variable within a value expression");
                    Log::PARSER->error("Did you mean to write `==` instead of `=`?");
                    frontToken->Indicate();
                    throw parse_error("");
                }
                else
                {
                    Program->Tokens.pop_front();
                    opToken->OpType = OperatorType::OperatorEq;
                    opToken->Position.Length++;
                }
            }
            else if (frontType == TokenType::ExclamationMark)
            {
                std::shared_ptr<Token> frontToken = Program->Tokens.front();
                if (Program->Tokens.front()->Type != TokenType::EqualSign)
                {
                    Log::PARSER->error("Cannot use postfix `!` on a value");
                    Log::PARSER->error("Did you mean to write `!=` instead of `!`?");
                    frontToken->Indicate();
                    throw parse_error("");
                }
                else
                {
                    Program->Tokens.pop_front();
                    opToken->OpType = OperatorType::OperatorNeq;
                    opToken->Position.Length++;
                }
            }
            else if (frontType == TokenType::CloseAngleBrace)
            {
                if (Program->Tokens.front()->Type == TokenType::EqualSign)
                {
                    Program->Tokens.pop_front();
                    opToken->OpType = OperatorType::OperatorGte;
                    opToken->Position.Length++;
                }
                else
                {
                    opToken->OpType = OperatorType::OperatorGt;
                }
            }
            else if (frontType == TokenType::OpenAngleBrace)
            {
                if (Program->Tokens.front()->Type == TokenType::EqualSign)
                {
                    Program->Tokens.pop_front();
                    opToken->OpType = OperatorType::OperatorLte;
                    opToken->Position.Length++;
                }
                else
                {
                    opToken->OpType = OperatorType::OperatorLt;
                }
            }

            // Comparator operators have the highest precedence
            // So we always just push
            operators.push_back(opToken);
            continue;
        }
		else
		{
			expressions.push_back(ConsumeNullaryOrUnaryValueExpression(depth));
			Log::PARSER->error(expressions.back()->ToHumanReadableString(""));
			lastTokenWasOperator = false;
		}
		
		
	}
	while (!operators.empty())
	{
		std::shared_ptr<OperatorToken> op = operators.back();
		operators.pop_back();
		if (expressions.size() < 2)
		{
			Log::PARSER->error("Expected value for binary operator '{}', instead got nothing", OperatorTypeNames[op->OpType]);
			op->Indicate();
			throw parse_error("");
		}
		std::shared_ptr<ValueExpression> right = expressions.back();
		expressions.pop_back();
		std::shared_ptr<ValueExpression> left = expressions.back();
		expressions.pop_back();
		expressions.push_back(std::make_shared<BinaryValueExpression>(op->OpType, left, right, op));
	}
	return std::move(expressions.front());
}

void VKParser::parse(TokenType endOfStatementToken)
{
    std::shared_ptr<Token> token = Program->Tokens.front();
    popToken();
    if (token->Type == endOfStatementToken)
    {
        return;
    }
    if (token->Type == TokenType::CloseCurlyBrace)
    {
        if (Program->ActiveScopes.size() == 1)
        {
            Log::PARSER->error("{}", Program->Source->Lines[token->Position.LineIndex]);
            std::string space = fmt::format("{: >{}}", ' ', token->Position.LineOffset);
            std::string tokenIndicator = fmt::format("{:^>{}}", ' ', token->Position.Length + 1);
            Log::PARSER->error(space + tokenIndicator);
            Log::PARSER->error("Unexpected scope end");
            Log::PARSER->error("Managed to parse the following tokens:");
            Program->printExpressionTree();
            throw parse_error("");
        }
        Program->ActiveScopes.pop_front();
        return;
    }
	if (token->Type == TokenType::Comment)
	{
		Program->ActiveScopes.front()->Expressions.push_back(std::make_unique<CommentExpression>(token));
		return;
	}
    if (token->Type == TokenType::Name)
    {
        TokenType nextType = Program->Tokens.front()->Type;
        /// ==========
        /// Declaration
        /// ==========
        if (nextType == TokenType::Name)
        {
            std::shared_ptr<Token> nameToken = expectToken(TokenType::Name);
            Program->ActiveScopes.front()->Expressions.push_back(std::make_unique<DeclarationExpression>(token->Value, nameToken->Value, token));
            Program->ActiveScopes.front()->Variables[nameToken->Value] = std::make_shared<Variable>(nameToken->Value, Program->FindType(token->Value));
            nextType = Program->Tokens.front()->Type;
            if (nextType != TokenType::EqualSign)
                return;
            // Set up for following assignment expression
            token = std::move(nameToken);
        }

        /// ==========
        /// Assignment
        /// ==========
        if (nextType == TokenType::EqualSign)
        {
            std::shared_ptr<Token> operatorToken = expectToken(TokenType::EqualSign);
            Program->ActiveScopes.front()->Expressions.push_back(std::make_unique<AssignmentExpression>(token->Value, parseValueExpression(0, endOfStatementToken), operatorToken));
            return;
        }

        /// ==========
        /// Function Call
        /// ==========
        if (nextType == TokenType::OpenParenthesis)
        {
            Program->Tokens.push_front(std::move(token));
            Program->ActiveScopes.front()->Expressions.push_back(parseValueExpression(0, TokenType::CloseParenthesis));
            return;
        }

    }
    if (token->Type == TokenType::Return)
    {
        if (Program->Tokens.front()->Type == TokenType::EndOfStatement)
        {
            popToken();
            Program->ActiveScopes.front()->Expressions.push_back(std::make_unique<ReturnExpression>(nullptr, Program->ActiveScopes.front()->ReturnType, token));
            return;
        }
        else
        {
            Program->ActiveScopes.front()->Expressions.push_back(std::make_unique<ReturnExpression>(parseValueExpression(0, TokenType::EndOfStatement), Program->ActiveScopes.front()->ReturnType, token));
            return;
        }

    }

    /// ==========
    /// Function Declaration
    /// ==========
    if (token->Type == TokenType::FunctionPrefix)
    {
        std::shared_ptr<Token> typeToken = expectToken(TokenType::Name);
        std::shared_ptr<Token> nameToken = expectToken(TokenType::Name);
        expectToken(TokenType::OpenParenthesis);
        std::vector<std::shared_ptr<FunctionParameter>> parameters;
        while (1)
        {
            if (Program->Tokens.front()->Type == TokenType::CloseParenthesis)
            {
                Program->Tokens.pop_front();
                break;
            }
            std::shared_ptr<Token> paramTypeToken = expectToken(TokenType::Name);
            std::shared_ptr<Token> paramNameToken = expectToken(TokenType::Name);
            parameters.push_back(std::make_shared<FunctionParameter>(paramNameToken->Value, Program->FindType(paramTypeToken->Value)));
            if (Program->Tokens.front()->Type == TokenType::CloseParenthesis)
            {
                Program->Tokens.pop_front();
                break;
            }
            expectToken(TokenType::CommaSeperator);
        }
        std::shared_ptr<FunctionObject> functionObject = std::make_shared<FunctionObject>(nameToken->Value, Program->FindType(typeToken->Value), parameters, Program->ActiveScopes.front());

        Program->ActiveScopes.front()->Expressions.push_back(std::make_shared<FunctionDeclarationExpression>(functionObject, nameToken));

        Program->Scopes.push_back(functionObject->FunctionScope);
        Program->ActiveScopes.push_front(functionObject->FunctionScope);
        Log::PARSER->debug("Adding scope '{}', scope count: {}", nameToken->Value, Program->ActiveScopes.size());

        Program->DefaultScope->Functions[functionObject->Name] = functionObject;
        Program->DefaultScope->Variables[functionObject->Name] = functionObject;

        expectToken(TokenType::OpenCurlyBrace);

        return;
    }
    if (token->Type == TokenType::IfStatement)
    {
        expectToken(TokenType::OpenParenthesis);
        std::shared_ptr<ValueExpression> condition = parseValueExpression(1, TokenType::CloseParenthesis);
        expectToken(TokenType::CloseParenthesis);
        expectToken(TokenType::OpenCurlyBrace);
        std::shared_ptr<IfStatementExpression> expression = std::make_shared<IfStatementExpression>(condition, Program->ActiveScopes.front(), token);
        std::shared_ptr<Scope> scope = expression->ScopeIfTrue;
        Program->ActiveScopes.front()->Expressions.push_back(expression);
        Program->ActiveScopes.push_front(scope);
        return;
    }
    if (token->Type == TokenType::ElseStatement)
    {
        if (Program->ActiveScopes.front()->Expressions.back()->Type != ExpressionType::If)
        {
            Log::PARSER->error("Cannot start an else-statement without a preceding if-statement");
            token->Indicate();
            throw parse_error("");
        }
        std::shared_ptr<IfStatementExpression> expr = dynamic_pointer_cast<IfStatementExpression>(Program->ActiveScopes.front()->Expressions.back());
        if (expr->HasElseClauseDefined)
        {
            Log::PARSER->error("Preceding if-statement already has an else clause defined");
            token->Indicate();
            throw parse_error("");
        }
        expr->HasElseClauseDefined = true;
        expectToken(TokenType::OpenCurlyBrace);
        std::shared_ptr<Scope> scope = expr->ScopeIfFalse;
        Program->ActiveScopes.push_front(scope);
        return;
    }
    if (token->Type == TokenType::ForStatement)
    {
        std::shared_ptr<Scope> parentScope = Program->ActiveScopes.front();
        expectToken(TokenType::OpenParenthesis);

        std::shared_ptr<Scope> innerScope = std::make_shared<Scope>("__impl_for", parentScope, parentScope->ReturnType);

        // Initializer
        std::shared_ptr<Token> typeToken = expectToken(TokenType::Name);
        std::shared_ptr<Token> nameToken = expectToken(TokenType::Name);
        expectToken(TokenType::EqualSign);
        std::shared_ptr<ValueExpression> initialValue = parseValueExpression(0, TokenType::EndOfStatement);
        expectToken(TokenType::EndOfStatement);
        innerScope->Variables[nameToken->Value] = std::make_shared<Variable>(nameToken->Value, Program->FindType(typeToken->Value));
        std::shared_ptr<AssignmentExpression> initializer = std::make_shared<AssignmentExpression>(nameToken->Value, initialValue, nameToken);
        Program->ActiveScopes.push_front(innerScope);

        // Conditional
        std::shared_ptr<ValueExpression> conditional = parseValueExpression(0, TokenType::EndOfStatement);
        expectToken(TokenType::EndOfStatement);

        // Increment
        parse(TokenType::CloseParenthesis);
        expectToken(TokenType::CloseParenthesis);
        std::shared_ptr<Expression> increment = Program->ActiveScopes.front()->Expressions.back();
        Program->ActiveScopes.front()->Expressions.pop_back();


        expectToken(TokenType::OpenCurlyBrace);

        std::shared_ptr<ForExpression> expression = std::make_shared<ForExpression>(initializer, conditional, increment, innerScope, token);
        parentScope->Expressions.push_back(expression);
        return;
    }

    else
    {
        Log::PARSER->error("Managed to parse the following tokens:");
        Program->printExpressionTree();
        token->Indicate();
        Log::PARSER->error("Failed to parse token with type '{}'", TokenTypeNames[token->Type]);
        throw parse_error("");
    }
}
}
