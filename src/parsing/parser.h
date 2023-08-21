#pragma once

class VKParser;

#include <vector>
#include "../core/token.h"
#include "../core/expression.h"
#include <string_view>
#include <functional>
#include <deque>
#include <memory>
#include <optional>
#include "../core/program.h"

#include "../core/namespace.h"

namespace Volk
{
class VKParser
{
public:
    Token lastConsumedToken;
    Program* Program;

public:

    void parse();
    std::unique_ptr<ValueExpression> parseValueExpression(int depth);
    std::unique_ptr<ValueExpression> ConsumeNullaryOrUnaryValueExpression(int depth);

    void visitExpression(Expression* expression, Scope* scope);

public:
    VKParser(Volk::Program* program) : lastConsumedToken(TokenType::EndOfStatement, "", {0,0,0})
    {
        Program = program;
    }

private:
    int readUntilNext(std::string_view& data, char character);
    int readWhile(std::string_view& data, std::function<bool(char)> predicate);

    void popToken();
    std::unique_ptr<Token> expectToken(TokenType type);
    std::optional<std::unique_ptr<Token>> softExpectToken(TokenType type);
};
}
