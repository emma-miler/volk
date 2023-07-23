#include "parser.h"
#include "../log/log.h"
#include "../core/keyword.h"
#include "../core/operator.h"

#include <spdlog/spdlog.h>
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
        charactersReadThisLine += read;
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
        Log::LEXER->debug("{}", expr->ToString());
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

bool isValidNameCharacter(char c)
{
    return std::isalpha(c) || c == '_';
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
    if (c == ' ') {
        Log::LEXER->trace("Skipping whitespace");
        charactersReadThisLine++;
        return 1;
    }
    if (c == '\n')
    {
        charactersReadThisLine = 0;
        lineIndex++;
        Log::LEXER->trace("Skipping whitespace");
        return 1;
    }

    /// ==========
    /// End of Statement
    /// ==========
    if (c == ';')
    {
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::EndOfStatement, data.substr(0, totalRead), currentPosition(totalRead)));
        charactersReadThisLine += totalRead;
        return totalRead;
    }

    /// ==========
    /// Names
    /// ==========
    if (isValidNameCharacter(c))
    {
        totalRead++;
        totalRead += readWhile(data, isValidNameCharacter);
        std::string name = std::string(data.substr(0, totalRead));
        auto keywordTokenType = KeywordLookup.find(name);
        if (keywordTokenType == KeywordLookup.end())
            Tokens.push_back(std::make_unique<Token>(TokenType::Name, data.substr(0, totalRead), currentPosition(totalRead)));
        else
            Tokens.push_back(std::make_unique<Token>(keywordTokenType->second, data.substr(0, totalRead), currentPosition(totalRead)));
        charactersReadThisLine += totalRead;
        return totalRead;
    }

    /// ==========
    /// Immediate Strings
    /// ==========
    if (c == '"')
    {
        totalRead++;
        totalRead += readUntilNext(data, '"');
        Tokens.push_back(std::make_unique<Token>(TokenType::ImmediateValue, data.substr(0, totalRead), currentPosition(totalRead)));
        charactersReadThisLine += totalRead;
        return totalRead;
    }

    /// ==========
    /// Immediate Numbers
    /// ==========
    if (std::isdigit(c))
    {
        totalRead++;
        totalRead += readWhile(data, isValidNumberCharacter);
        Tokens.push_back(std::make_unique<Token>(TokenType::ImmediateValue, data.substr(0, totalRead), currentPosition(totalRead)));
        charactersReadThisLine += totalRead;
        return totalRead;
    }

    /// ==========
    /// Operators
    /// ==========
    if (c == '=')
    {
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::Assignment, data.substr(0, totalRead), currentPosition(totalRead)));
        charactersReadThisLine += totalRead;
        return totalRead;
    }

    /// ==========
    /// Binary Operators
    /// ==========
    auto op = OperatorTypeLookup.find(std::string(1, c));
    if (op != OperatorTypeLookup.end())
    {
        totalRead++;
        Tokens.push_back(std::make_unique<Token>(TokenType::Operator, data.substr(0, totalRead), currentPosition(totalRead)));
        charactersReadThisLine += totalRead;
        return totalRead;
    }

    printCurrentTokens();
    Log::LEXER->error("{}", Lines[lineIndex]);
    Log::LEXER->error("{: >{}}", '^', charactersReadThisLine);
    Log::LEXER->error("Failed to parse character '{}' (0x{:0x}) Bailing", c, c);
    exit(1);
}

std::unique_ptr<ValueExpression> VKParser::parseValueExpression()
{
    std::unique_ptr<Token> token = std::move(Tokens.front());
    Tokens.pop_front();

    // Pop tokens until we find an end-of-statement
    std::deque<std::unique_ptr<Token>> tokens;
    tokens.push_back(std::move(token));
    while (!tokens.empty())
    {
        if (Tokens.front()->Type == TokenType::EndOfStatement)
            break;
        tokens.push_back(std::move(Tokens.front()));
        Tokens.pop_front();
    }
    Log::LEXER->debug("Popped {} tokens while parsing value expression", tokens.size());
    for (auto&& token : tokens)
    {
        Log::LEXER->debug("{:16s} : '{}'", TokenTypeNames[token->Type], token->Value);
    }

    if (tokens.size() == 1)
    {
        return std::make_unique<ValueExpression>(tokens.front()->Value);
    }
    // This should maybe be implemented as a stack machine
    // Where 1 token is popped at a time, and depending on the type
    // either 1 or 2 more are consumed, then pushed to the front of the queue
    // Operator precedence is a spook anyway, just use parens
    while (tokens.size() != 0)
    {
         // Operators may appear at the start of a value expression
        // but only if they are unary
        if (tokens.front()->Type == TokenType::Operator)
        {
            OperatorToken operatorToken = *static_cast<OperatorToken*>(tokens.front().get());
            tokens.pop_front();
            // Check if operator is unary
            auto unaryOperator = std::find(UnaryOperators.begin(), UnaryOperators.end(), operatorToken.OpType);
            if (unaryOperator == UnaryOperators.end())
            {
                Log::LEXER->critical("Found non-unary operator at start of value expression");
                throw std::format_error("");
            }
        }
    }

    throw std::format_error("");
}


void VKParser::parse()
{
    std::unique_ptr<Token> token = std::move(Tokens.front());
    Tokens.pop_front();
    if (token->Type == TokenType::EndOfStatement)
    {
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
            std::unique_ptr<Token> nameToken = std::move(Tokens.front());
            Tokens.pop_front();
            Expressions.push_back(std::make_unique<DeclarationExpression>(token->Value, nameToken->Value));

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
            std::unique_ptr<Token> operatorToken = std::move(Tokens.front());
            Tokens.pop_front();
            if (operatorToken->Value == "=")
            {
                Expressions.push_back(std::make_unique<AssignmentExpression>(token->Value, parseValueExpression()));
                return;
            }
            else
            {
                throw std::format_error("");
            }
        }
    }
    if (token->Type == TokenType::Return)
    {
        Expressions.push_back(std::make_unique<ReturnExpression>(parseValueExpression()));
    }
    else
    {
        Log::LEXER->error("Managed to parse the following tokens:");
        printExpressionTree();
        Log::LEXER->error("{}", Lines[token->Position.LineIndex]);
        Log::LEXER->error("{: >{}}", '^', token->Position.LineOffset);
        Log::LEXER->error("Failed to parse token with type '{}'", TokenTypeNames[token->Type]);
        throw std::format_error("");
    }
}

}
