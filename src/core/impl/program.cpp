#include "../program.h"

namespace Volk
{
void Program::printCurrentTokens()
{
    Log::LEXER->debug("Read {} tokens: ", Tokens.size());
    for (auto&& token : Tokens)
    {
        if (token->Type == TokenType::StringConstant)
        {
            Log::LEXER->debug("{:16s} : '{}'", TokenTypeNames[token->Type], string_sanitize(StringTable[atoi(token->Value.c_str())]));
            continue;
        }
        Log::LEXER->debug("{:16s} : '{}'", TokenTypeNames[token->Type], token->Value);
    }
}

void Program::printStringTable()
{
    Log::PARSER->debug("String table ({}): ", StringTable.size());
    for (int i = 0; i < StringTable.size(); i++)
    {
        Log::PARSER->debug("{} ({}): '{}'", i, StringTable[i].length(), string_sanitize(StringTable[i]));
    }
}

void Program::printExpressionTree()
{
    Log::PARSER->debug("Read {} expressions: ", Expressions.size());
    for (auto&& expr : Expressions)
    {
        Log::PARSER->debug("{}", expr->ToString());
    }
}

std::shared_ptr<Type> Program::FindType(std::string typeName)
{
    // TODO: remove this evil temporary hack
    if (typeName == "string") typeName = "raw_ptr";
    for (auto&& scope : ActiveScopes)
    {
        // Try to find the type name in the scope
        auto it = scope->Types.find(typeName);
        // Didn't find it, continue to next scope up
        if (it == scope->Types.end()) continue;
        // If we did find it, return it
        return it->second;
    }
    Log::PARSER->error(fmt::format("Unknown typename '{}'", typeName));
    throw type_error("");
}
}
