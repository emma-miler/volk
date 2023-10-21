#include "../program.h"

namespace Volk
{

Program::Program()
{
    Source = std::make_shared<ProgramSource>();
    DefaultScope = std::make_shared<Scope>("__root_scope", nullptr, BUILTIN_INT);
    Scopes.push_back(DefaultScope);
    ActiveScopes.push_front(DefaultScope);
    DefaultScope->AddBuiltinTypes();
    std::vector<std::shared_ptr<FunctionParameter>> printf_params;
    printf_params.push_back(std::make_shared<FunctionParameter>("format_string", BUILTIN_STRING));
    printf_params.push_back(std::make_shared<FunctionParameter>("args", BUILTIN_VARARGS));
    auto func_printf = std::make_shared<FunctionObject>("printf", BUILTIN_INT, printf_params, DefaultScope);
    DefaultScope->AddVariable(func_printf);
}

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
    Log::PARSER->debug("Read {} expressions: ", DefaultScope->Expressions.size());
    for (auto&& expr : DefaultScope->Expressions)
    {
        Log::PARSER->debug("{}", expr->ToString());
    }
	
	for (auto&& func : DefaultScope->Functions)
    {	
		Volk::Log::PARSER->debug("Function: '{}'", func.first);
		Volk::Log::PARSER->debug(func.second->ToHumanReadable());
        /*for (auto&& expr : func.second->FunctionScope->Expressions)
		{
			Log::PARSER->debug("{}", expr->ToString());
		}*/
    }
}

std::shared_ptr<VKType> Program::FindType(std::string typeName)
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
