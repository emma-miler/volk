#pragma once

#include <optional>

#include "../common.h"
#include "../core/token.h"
#include "../core/expression.h"
#include "../core/program.h"


namespace Volk
{
class VKParser
{
public:
    Token lastConsumedToken;
    Volk::Program* Program;

public:

    void parse(TokenType endOfStatementToken);
    std::shared_ptr<ValueExpression> parseValueExpression(int depth, TokenType endMarker);
    std::shared_ptr<ValueExpression> ConsumeNullaryOrUnaryValueExpression(int depth);

    void parserPass_NameResolution(Scope* scope);
    void parserPass_TypeChecking(Scope* scope);

public:
    VKParser(Volk::Program* program) : lastConsumedToken(TokenType::EndOfStatement, "", {0,0,0})
    {
        Program = program;
    }

private:
    int readUntilNext(std::string_view& data, char character);
    int readWhile(std::string_view& data, std::function<bool(char)> predicate);

    void parserPass_NameResolution(std::shared_ptr<Expression> expression, Scope* scope);
    void parserPass_TypeChecking(std::shared_ptr<Expression> expression, Scope* scope);

    void popToken();
    std::shared_ptr<Token> expectToken(TokenType type);
    std::optional<std::shared_ptr<Token>> softExpectToken(TokenType type);
};
}
