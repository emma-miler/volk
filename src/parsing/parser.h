#pragma once

class VKParser;

#include <vector>
#include "../core/token.h"
#include "../core/expression.h"
#include <string_view>
#include <functional>
#include <deque>
#include <memory>

namespace Volk
{
class VKParser
{
public:
    std::string Source;
    std::deque<std::unique_ptr<Token>> Tokens;
    std::vector<std::unique_ptr<Expression>> Expressions;
    Token lastConsumedToken;
    int charactersReadThisLine = 0;
    int lineIndex = 0;
    std::vector<std::string> Lines;


public:
    void consume(std::string data);
    void printCurrentTokens();
    void printExpressionTree();

    void parse();
    std::unique_ptr<ValueExpression> parseValueExpression();

public:
    VKParser() : lastConsumedToken(TokenType::EndOfStatement, "", {0,0,0}) {};

private:
    SourcePosition currentPosition(int length);
    // Returns amount of characters read
    int readToken(std::string_view data);

    int readUntilNext(std::string_view& data, char character);
    int readWhile(std::string_view& data, std::function<bool(char)> predicate);
};
}
