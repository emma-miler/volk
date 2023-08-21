#include "parser.h"
#include "../log/log.h"
#include "../core/keyword.h"
#include "../core/operator.h"
#include "../core/exceptions.h"
#include "../util/string.h"

#include <spdlog/spdlog.h>

#include <optional>

namespace Volk
{

void VKParser::popToken()
{
    Program->Tokens.pop_front();
}


std::unique_ptr<Token> VKParser::expectToken(TokenType expectedType)
{
    std::unique_ptr<Token> token = std::move(Program->Tokens.front()); popToken();
    if (token->Type != expectedType)
    {
        Log::PARSER->error("{}", Program->Lines[token->Position.LineIndex]);
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

std::optional<std::unique_ptr<Token>> VKParser::softExpectToken(TokenType expectedType)
{
    std::unique_ptr<Token> token = std::move(Program->Tokens.front()); popToken();
    if (token->Type != expectedType)
    {
        return std::nullopt;
    }
    return token;
}


std::unique_ptr<ValueExpression> VKParser::ConsumeNullaryOrUnaryValueExpression(int depth)
{
    ValueExpressionType exprType = ValueExpressionType::Unary;
    OperatorType opType = OperatorType::Null;

    /// ==========
    /// Nested expression
    /// ==========
    // TODO: need to check for function here
    // Maybe need to add a context variable to this function's arguments
    if (Program->Tokens.front()->Type == TokenType::OpenExpressionScope)
    {
        expectToken(TokenType::OpenExpressionScope);
        auto temp = parseValueExpression(depth + 1);
        expectToken(TokenType::CloseExpressionScope);
        return temp;
    }

    /// ==========
    /// Unary Operator
    /// ==========
    if (Program->Tokens.front()->Type == TokenType::Operator)
    {
        OperatorToken operatorToken = *static_cast<OperatorToken*>(Program->Tokens.front().get()); popToken();
        // Check if operator is unary
        auto unaryOperator = std::find(UnaryOperators.begin(), UnaryOperators.end(), operatorToken.OpType);
        Log::PARSER->trace("{}", (void*)unaryOperator);
        Log::PARSER->trace("{}", (void*)UnaryOperators.end());
        Log::PARSER->trace("{}", operatorToken.ToString());
        if (unaryOperator == UnaryOperators.end())
        {
            Log::PARSER->critical("Found non-unary operator '{}' at start of value expression", operatorToken.ToString());
            throw parse_error("");
        }
        exprType = ValueExpressionType::Unary;
        opType = *unaryOperator;
    }
    // I have no clue what's going on here tbh
    Token token = *static_cast<OperatorToken*>(Program->Tokens.front().get()); popToken();
    std::unique_ptr<ValueExpression> expr;
    if (token.Type == TokenType::ImmediateValue)
    {
        expr = std::make_unique<ImmediateValueExpression>(token.Value);
    }
    else if (token.Type == TokenType::Name)
    {
        expr = std::make_unique<IndirectValueExpression>(token.Value);
    }
    else if (token.Type == TokenType::StringConstant)
    {
        expr = std::make_unique<StringConstantValueExpression>(token.Value);
    }
    else
    {
        Log::PARSER->critical("Unexpected token {} in while parsing ConsumeNullaryOrUnaryValueExpression", token.ToString());
        throw parse_error("");
    }
    exprType = expr->ValueType;

    if (exprType == ValueExpressionType::StringConstant || exprType == ValueExpressionType::Immediate || exprType == ValueExpressionType::Indirect)
    {
        return expr;
    }
    else if (exprType == ValueExpressionType::Unary)
    {
        return std::make_unique<UnaryValueExpression>(opType, std::move(expr));
    }
    Log::PARSER->critical(fmt::format("Unknown ValueExpressionType in ConsumeNullaryOrUnaryValueExpression: '{}'", (int)exprType));
    throw parse_error("");

}

std::unique_ptr<ValueExpression> VKParser::parseValueExpression(int depth)
{
    Log::PARSER->trace("parseValueExpression depth = {}", depth);
    Log::PARSER->trace("front token = {}", Program->Tokens.front()->ToString());
    std::optional<std::unique_ptr<ValueExpression>> leftHandSide = std::nullopt;
    std::optional<std::unique_ptr<ValueExpression>> rightHandSide = std::nullopt;
    OperatorToken operatorToken = OperatorToken::Dummy();

    bool isExpectingOperator = false;

    while (Program->Tokens.size() != 0)
    {
        // If we are the top level of a nested expression, we look for an EndOfStatement token
        // Otherwise, we look for a CloseExpressionScope token, since we must first close all
        // the open ExpressionScopes, aka every '(' must have a matching ')'
        if ((depth == 0 && Program->Tokens.front()->Type == TokenType::EndOfStatement) ||
            (depth > 0 && Program->Tokens.front()->Type == TokenType::CloseExpressionScope))
        {
            break;
        }
        if (isExpectingOperator)
        {
            // We have already parsed the left-hand side of a possibly multivalued expression
            // This means we can either find a binary operator, or a function call
            if (Program->Tokens.front()->Type == TokenType::Operator)
            {
                operatorToken = *static_cast<OperatorToken*>(Program->Tokens.front().get()); popToken();
                isExpectingOperator = false;
                continue;
            }
            else if (Program->Tokens.front()->Type == TokenType::OpenExpressionScope)
            {
                std::string funcName = (*static_cast<IndirectValueExpression*>(leftHandSide->get())).Value;
                std::unique_ptr<FunctionCallValueExpression> func = std::make_unique<FunctionCallValueExpression>(funcName);
                popToken();
                int i = 0;
                while (Program->Tokens.front()->Type != TokenType::CloseExpressionScope)
                {
                    func->Arguments.push_back(ConsumeNullaryOrUnaryValueExpression(depth));
                    Log::PARSER->trace("func argument {}", i);
                    i++;
                    if (!softExpectToken(TokenType::CommaSeperator).has_value())
                        break;
                }
                leftHandSide = std::move(func);
                isExpectingOperator = true;
                continue;
            }

            Log::PARSER->critical("Found unexpected token while parsing");
            Log::PARSER->critical("Expected either 'Operator' or 'OpenExpressionScope', instead got '{}'", TokenTypeNames[Program->Tokens.front()->Type]);
            Log::PARSER->critical("{}", Program->Tokens.front()->ToString());
            throw parse_error("");


        }
        Log::PARSER->trace("getting value expression");
        if (!leftHandSide.has_value())
        {
            Log::PARSER->trace("getting left hand");
            leftHandSide = ConsumeNullaryOrUnaryValueExpression(depth);
            isExpectingOperator = true;
            continue;
        }
        Log::PARSER->trace("getting right hand");
        rightHandSide = ConsumeNullaryOrUnaryValueExpression(depth);
        leftHandSide = std::make_unique<BinaryValueExpression>(
                operatorToken.OpType,
                std::move(leftHandSide.value()),
                std::move(rightHandSide.value())
        );
        isExpectingOperator = true;

        Log::PARSER->trace("got value expression");
        Log::PARSER->trace("{}", leftHandSide.value()->ToString());
    }
    if (leftHandSide.has_value())
    {
        return std::move(leftHandSide.value());
    }
    else
    {
        Log::PARSER->critical("Unexpected EndOfExpression hit while parsing");
        throw parse_error("");
    }
}

void VKParser::parse()
{
    std::unique_ptr<Token> token = std::move(Program->Tokens.front());
    popToken();
    if (token->Type == TokenType::EndOfStatement)
    {
        return;
    }
    if (token->Type == TokenType::CloseScope)
    {
        if (Program->ActiveScopes.size() == 1)
        {
            Log::PARSER->error("{}", Program->Lines[token->Position.LineIndex]);
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
    if (token->Type == TokenType::Name)
    {
        TokenType nextType = Program->Tokens.front()->Type;
        /// ==========
        /// Declaration
        /// ==========
        if (nextType == TokenType::Name)
        {
            std::unique_ptr<Token> nameToken = expectToken(TokenType::Name);
            Program->Scopes.front()->Expressions.push_back(std::make_unique<DeclarationExpression>(token->Value, nameToken->Value));
            Program->Scopes.front()->Variables[nameToken->Value] = std::make_shared<Variable>(nameToken->Value, Program->FindType(token->Value));
            nextType = Program->Tokens.front()->Type;
            if (nextType != TokenType::Assignment)
                return;
            // Set up for following assignment expression
            token = std::move(nameToken);
        }

        /// ==========
        /// Assignment
        /// ==========
        if (nextType == TokenType::Assignment)
        {
            std::unique_ptr<Token> operatorToken = expectToken(TokenType::Assignment);
            Program->Scopes.front()->Expressions.push_back(std::make_unique<AssignmentExpression>(token->Value, parseValueExpression(0)));
            return;
        }

        /// ==========
        /// Function Call
        /// ==========
        if (nextType == TokenType::OpenExpressionScope)
        {
            Program->Tokens.push_front(std::move(token));
            Program->Scopes.front()->Expressions.push_back(parseValueExpression(0));
            return;
        }

    }
    if (token->Type == TokenType::Return)
    {
        Program->Scopes.front()->Expressions.push_back(std::make_unique<ReturnExpression>(parseValueExpression(0)));
        return;
    }

    /// ==========
    /// Function Declaration
    /// ==========
    if (token->Type == TokenType::FunctionPrefix)
    {
        std::unique_ptr<Token> typeToken = expectToken(TokenType::Name);
        std::unique_ptr<Token> nameToken = expectToken(TokenType::Name);
        expectToken(TokenType::OpenExpressionScope);
        std::unique_ptr<FunctionObject> functionObject = std::make_unique<FunctionObject>(nameToken->Value, typeToken->Value, Program->ActiveScopes.front());
        while (1)
        {
            if (Program->Tokens.front()->Type == TokenType::CloseExpressionScope)
            {
                Program->Tokens.pop_front();
                break;
            }
            std::unique_ptr<Token> paramTypeToken = expectToken(TokenType::Name);
            std::unique_ptr<Token> paramNameToken = expectToken(TokenType::Name);
            functionObject->Parameters.push_back(FunctionParameter{paramNameToken->Value, paramTypeToken->Value});
            if (Program->Tokens.front()->Type == TokenType::CloseExpressionScope)
            {
                Program->Tokens.pop_front();
                break;
            }
            expectToken(TokenType::CommaSeperator);
        }
        Program->Scopes.push_back(functionObject->FunctionScope);
        Program->ActiveScopes.push_front(functionObject->FunctionScope);

        Program->RootNamespace->Functions.push_back(std::move(functionObject));

        expectToken(TokenType::OpenScope);

        return;
    }

    else
    {
        Log::PARSER->error("Managed to parse the following tokens:");
        Program->printExpressionTree();
        Log::PARSER->error("{}", Program->Lines[token->Position.LineIndex]);
        Log::PARSER->error("{: >{}}", '^', token->Position.LineOffset);
        Log::PARSER->error("Failed to parse token with type '{}'", TokenTypeNames[token->Type]);
        throw parse_error("");
    }
}




void VKParser::visitExpression(Expression* expression, Scope* scope)
{
    Log::PARSER->error("visiting node");
    if (expression->Type == ExpressionType::Value)
    {
        ValueExpression* valueExpression = (ValueExpression*)expression;
        if (valueExpression->ValueType == ValueExpressionType::Indirect)
        {
            IndirectValueExpression* expr = (IndirectValueExpression*)expression;
            expr->Variable = scope->FindVariable(expr->Value);
            Log::PARSER->error("visited node");
        }
    }
    for (auto&& expr : expression->SubExpressions())
    {
        visitExpression(expr, scope);
    }
}


}
