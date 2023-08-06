#include "parser.h"
#include "../log/log.h"
#include "../core/keyword.h"
#include "../core/operator.h"
#include "../core/exceptions.h"

#include <spdlog/spdlog.h>

#include <optional>

namespace Volk
{

SourcePosition VKParser::currentPosition(int length)
{
    return {
        .LineIndex = lineIndex,
        .LineOffset = charactersReadThisLine,
        .Length = length
    };
}

void VKParser::consume(std::string file)
{
    Source = file;
    std::stringstream ss(Source);
    std::string temp;
    while (std::getline(ss, temp, '\n')) {
        Lines.push_back(temp);
    }
    std::string_view content = file;
    while (1)
    {
        int read = readToken(content);
        if (read == -1)
        {
            printCurrentTokens();
            return;
        }
        Log::FRONTEND->debug("read {} chars", read);
        content = content.substr(read);
    }
}

void VKParser::printCurrentTokens()
{
    Log::LEXER->debug("Read {} tokens: ", Tokens.size());
    for (auto&& token : Tokens)
    {
        Log::LEXER->debug("{:16s} : '{}'", TokenTypeNames[token->Type], token->Value);
    }
}

void VKParser::printExpressionTree()
{
    Log::LEXER->debug("Read {} expressions: ", Expressions.size());
    for (auto&& expr : Expressions)
    {
        Log::PARSER->debug("{}", expr->ToString());
    }
}

int VKParser::readUntilNext(std::string_view& data, char character)
{
    int needle = data.find(character);
    if (needle == std::string::npos) {
        Log::LEXER->error("Reached end of string while parsing. Bailing");
        exit(1);
    }
    return needle;
}

int VKParser::readWhile(std::string_view& data, std::function<bool(char)> predicate)
{
    int totalRead = 0;
    for (const char& c : data)
    {
        if (!predicate(c))
        {
            return totalRead - 1;
        }
        totalRead++;
    }
    Log::LEXER->error("Reached end of string while parsing. Bailing");
    exit(1);
}

bool isValidNameFirstCharacter(char c)
{
    return std::isalpha(c) || c == '_';
}

bool isValidNameCharacter(char c)
{
    return std::isalnum(c) || c == '_';
}

bool isValidNumberCharacter(char c)
{
    return std::isdigit(c) || c == '.';
}

int VKParser::readToken(std::string_view data)
{
    int totalRead = 0;
    int strLen = data.length();
    char c = data[0];
    if (c == '\0')
    {
        Log::LEXER->debug("End of file!");
        return -1;
    }
    else if (c == ' ') {
        Log::LEXER->trace("Skipping whitespace");
        //charactersReadThisLine++;
        totalRead++;
    }
    else if (c == '\n')
    {
        Log::LEXER->trace("Skipping newline");
        charactersReadThisLine = 0;
        lineIndex++;
        totalRead++;
    }

    /// ==========
    /// End of Statement
    /// ==========
    else if (c == ';')
    {
        Log::LEXER->trace("Read EOS");
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::EndOfStatement, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Names
    /// ==========
    else if (isValidNameFirstCharacter(c))
    {
        Log::LEXER->trace("Read name");
        totalRead++;
        totalRead += readWhile(data, isValidNameCharacter);
        std::string name = std::string(data.substr(0, totalRead));
        auto keywordTokenType = KeywordLookup.find(name);
        if (keywordTokenType == KeywordLookup.end())
            Tokens.push_back(std::make_unique<Token>(TokenType::Name, data.substr(0, totalRead), currentPosition(totalRead)));
        else
            Tokens.push_back(std::make_unique<Token>(keywordTokenType->second, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Immediate Strings
    /// ==========
    else if (c == '"')
    {
        Log::LEXER->trace("Read string");
        totalRead++;
        totalRead += readUntilNext(data, '"');
        Tokens.push_back(std::make_unique<Token>(TokenType::ImmediateValue, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Expression Scope
    /// ==========
    else if (c == '(')
    {
        Log::LEXER->trace("Read ExprScopeOpen");
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::OpenExpressionScope, data.substr(0, totalRead), currentPosition(totalRead)));
    }
    else if (c == ')')
    {
        Log::LEXER->trace("Read ExprScopeClose");
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::CloseExpressionScope, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Scope
    /// ==========
    else if (c == '{')
    {
        Log::LEXER->trace("Read ScopeOpen");
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::OpenScope, data.substr(0, totalRead), currentPosition(totalRead)));
    }
    else if (c == '}')
    {
        Log::LEXER->trace("Read ScopeClose");
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::CloseScope, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Immediate Numbers
    /// ==========
    else if (std::isdigit(c))
    {
        Log::LEXER->trace("Read number");
        totalRead++;
        totalRead += readWhile(data, isValidNumberCharacter);
        Tokens.push_back(std::make_unique<Token>(TokenType::ImmediateValue, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Assignment Operator
    /// ==========
    else if (c == '=')
    {
        Log::LEXER->trace("Read Assign");
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::Assignment, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Binary Operators
    /// ==========
    else if (OperatorTypeLookup.find(std::string(1, c)) != OperatorTypeLookup.end())
    {
        Log::LEXER->trace("Read BinaryOperator");
        totalRead++;
        Tokens.push_back(std::make_unique<OperatorToken>(data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Comma Seperator
    /// ==========
    else if (c == ',')
    {
        Log::LEXER->trace("Read Comma");
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::CommaSeperator, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    else
    {
        printCurrentTokens();
        Log::LEXER->error("{}", Lines[lineIndex]);
        Log::LEXER->error("{: >{}}", '^', charactersReadThisLine + 1);
        Log::LEXER->error("Failed to parse character '{}' (0x{:0x}) Bailing", c, c);
        exit(1);
    }
    charactersReadThisLine += totalRead;
    return totalRead;

}

std::unique_ptr<ValueExpression> VKParser::ConsumeNullaryOrUnaryValueExpression(int depth)
{
    ValueExpressionType exprType = ValueExpressionType::Nullary;
    OperatorType opType = OperatorType::Null;

    /// ==========
    /// Nested expression
    /// ==========
    if (Tokens.front()->Type == TokenType::OpenExpressionScope)
    {
        Tokens.pop_front();
        auto temp = parseValueExpression(depth + 1);
        Tokens.pop_front();
        return temp;
    }

    /// ==========
    /// Unary Operator
    /// ==========
    if (Tokens.front()->Type == TokenType::Operator)
    {
        OperatorToken operatorToken = *static_cast<OperatorToken*>(Tokens.front().get());
        Tokens.pop_front();
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

    Token token = *static_cast<OperatorToken*>(Tokens.front().get());
    Tokens.pop_front();
    std::unique_ptr<ValueExpression> expr;
    if (token.Type == TokenType::ImmediateValue)
    {
        expr = std::make_unique<ImmediateValueExpression>(token.Value);
    }
    else if (token.Type == TokenType::Name)
    {
        expr = std::make_unique<IndirectValueExpression>(token.Value);
    }
    else
    {
        Log::PARSER->critical("Unexpected token {} in while parsing ConsumeNullaryOrUnaryValueExpression", token.ToString());
        throw parse_error("");
    }

    if (exprType == ValueExpressionType::Nullary)
    {
        return expr;
    }
    else if (exprType == ValueExpressionType::Unary)
    {
        return std::make_unique<UnaryValueExpression>(opType, std::move(expr));
    }
    Log::PARSER->critical("Unknown ValueExpressionType in ConsumeNullaryOrUnaryValueExpression");
    throw parse_error("");

}

std::unique_ptr<ValueExpression> VKParser::parseValueExpression(int depth)
{
    std::optional<std::unique_ptr<ValueExpression>> leftHandSide = std::nullopt;
    std::optional<std::unique_ptr<ValueExpression>> rightHandSide = std::nullopt;
    OperatorToken operatorToken = OperatorToken::Dummy();

    bool isExpectingOperator = false;

    while (Tokens.size() != 0)
    {
        if ((depth == 0 && Tokens.front()->Type == TokenType::EndOfStatement) ||
            (depth > 0 && Tokens.front()->Type == TokenType::CloseExpressionScope))
        {
            break;
        }
        if (isExpectingOperator)
        {
            if (Tokens.front()->Type != TokenType::Operator)
            {
                Log::PARSER->critical("Found non-operator token while expected in value expression");
                Log::PARSER->critical("{}", Tokens.front()->ToString());
                throw parse_error("");
            }

            operatorToken = *static_cast<OperatorToken*>(Tokens.front().get());
            Tokens.pop_front();
            isExpectingOperator = false;
            continue;
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

std::unique_ptr<Token> VKParser::expectToken(TokenType expectedType)
{
    std::unique_ptr<Token> token = std::move(Tokens.front()); Tokens.pop_front();
    if (token->Type != expectedType)
    {
        Log::PARSER->error("{}", Lines[token->Position.LineIndex]);
        std::string space = fmt::format("{: >{}}", ' ', token->Position.LineOffset);
        std::string tokenIndicator = fmt::format("{:^>{}}", ' ', token->Position.Length + 1);
        Log::PARSER->error(space + tokenIndicator);
        Log::PARSER->error("Expected token type '{}', instead found '{}'", TokenTypeNames[expectedType], TokenTypeNames[token->Type]);
        Log::PARSER->error("Managed to parse the following tokens:");
        printExpressionTree();

        throw parse_error("");
    }
    return token;
}

void VKParser::parse()
{
    std::unique_ptr<Token> token = std::move(Tokens.front());
    Tokens.pop_front();
    if (token->Type == TokenType::EndOfStatement)
    {
        return;
    }
    if (token->Type == TokenType::CloseScope)
    {
        if (Scopes.size() == 1)
        {
            Log::PARSER->error("{}", Lines[token->Position.LineIndex]);
            std::string space = fmt::format("{: >{}}", ' ', token->Position.LineOffset);
            std::string tokenIndicator = fmt::format("{:^>{}}", ' ', token->Position.Length + 1);
            Log::PARSER->error(space + tokenIndicator);
            Log::PARSER->error("Unexpected scope end");
            Log::PARSER->error("Managed to parse the following tokens:");
            printExpressionTree();
            throw parse_error("");
        }
        Scopes.pop_front();
        return;
    }
    if (token->Type == TokenType::Name)
    {
        TokenType nextType = Tokens.front()->Type;
        /// ==========
        /// Declaration
        /// ==========
        if (nextType == TokenType::Name)
        {
            std::unique_ptr<Token> nameToken = expectToken(TokenType::Name);
            Scopes.front()->Expressions.push_back(std::make_unique<DeclarationExpression>(token->Value, nameToken->Value));

            nextType = Tokens.front()->Type;
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
            Scopes.front()->Expressions.push_back(std::make_unique<AssignmentExpression>(token->Value, parseValueExpression(0)));
            return;
        }
    }
    if (token->Type == TokenType::Return)
    {
        Scopes.front()->Expressions.push_back(std::make_unique<ReturnExpression>(parseValueExpression(0)));
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
        std::unique_ptr<FunctionObject> functionObject = std::make_unique<FunctionObject>(nameToken->Value, typeToken->Value);
        while (1)
        {
            if (Tokens.front()->Type == TokenType::CloseExpressionScope)
            {
                Tokens.pop_front();
                break;
            }
            std::unique_ptr<Token> paramTypeToken = expectToken(TokenType::Name);
            std::unique_ptr<Token> paramNameToken = expectToken(TokenType::Name);
            if (Tokens.front()->Type == TokenType::CloseExpressionScope)
            {
                Tokens.pop_front();
                functionObject->Parameters.push_back(FunctionParameter{paramNameToken->Value, paramTypeToken->Value});
                break;
            }
            expectToken(TokenType::CommaSeperator);
        }

        Scopes.push_front(functionObject->FunctionScope);

        RootNamespace->Functions.push_back(std::move(functionObject));

        expectToken(TokenType::OpenScope);

        return;
    }

    else
    {
        Log::PARSER->error("Managed to parse the following tokens:");
        printExpressionTree();
        Log::PARSER->error("{}", Lines[token->Position.LineIndex]);
        Log::PARSER->error("{: >{}}", '^', token->Position.LineOffset);
        Log::PARSER->error("Failed to parse token with type '{}'", TokenTypeNames[token->Type]);
        throw parse_error("");
    }
}

}
