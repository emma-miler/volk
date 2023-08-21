#include "lexer.h"

#include "../core/program.h"
#include "../core/keyword.h"


namespace Volk
{

int charactersReadThisLine = 0;
int lineIndex = 0;
SourcePosition currentPosition(int length)
{
    return {
        .LineIndex = lineIndex,
        .LineOffset = charactersReadThisLine,
        .Length = length
    };
}


int readUntilNext(std::string_view& data, char character)
{
    int needle = data.find(character);
    if (needle == std::string::npos) {
        Log::LEXER->error("Reached end of string while parsing. Bailing");
        exit(1);
    }
    return needle;
}

int readWhile(std::string_view& data, std::function<bool(char)> predicate)
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


int readToken(std::string_view data, std::deque<std::unique_ptr<Token>>& tokens, Program& program)
{
    int totalRead = 0;
    int strLen = data.length();
    char c = data[0];
    if (c == '\0')
    {
        Log::LEXER->debug("End of file!");
        return -1;
    }
    else if (c == ' ' || c == '\t') {
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
        tokens.push_back(std::make_unique<Token>(TokenType::EndOfStatement, data.substr(0, totalRead), currentPosition(totalRead)));
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
            tokens.push_back(std::make_unique<Token>(TokenType::Name, data.substr(0, totalRead), currentPosition(totalRead)));
        else
            tokens.push_back(std::make_unique<Token>(keywordTokenType->second, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Immediate Strings
    /// ==========
    else if (c == '"')
    {
        Log::LEXER->trace("Read string");
        totalRead++;
        std::string_view toRead = data.substr(1, data.length() - 1);
        totalRead += readUntilNext(toRead, '"') - 1;
        tokens.push_back(std::make_unique<StringToken>(data.substr(1, totalRead), currentPosition(totalRead), program.StringTable));

        totalRead += 2;
    }

    /// ==========
    /// Expression Scope
    /// ==========
    else if (c == '(')
    {
        Log::LEXER->trace("Read ExprScopeOpen");
        totalRead++;
        tokens.push_back(std::make_unique<Token>(TokenType::OpenExpressionScope, data.substr(0, totalRead), currentPosition(totalRead)));
    }
    else if (c == ')')
    {
        Log::LEXER->trace("Read ExprScopeClose");
        totalRead++;
        tokens.push_back(std::make_unique<Token>(TokenType::CloseExpressionScope, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Scope
    /// ==========
    else if (c == '{')
    {
        Log::LEXER->trace("Read ScopeOpen");
        totalRead++;
        tokens.push_back(std::make_unique<Token>(TokenType::OpenScope, data.substr(0, totalRead), currentPosition(totalRead)));
    }
    else if (c == '}')
    {
        Log::LEXER->trace("Read ScopeClose");
        totalRead++;
        tokens.push_back(std::make_unique<Token>(TokenType::CloseScope, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Immediate Numbers
    /// ==========
    else if (std::isdigit(c))
    {
        Log::LEXER->trace("Read number");
        totalRead++;
        totalRead += readWhile(data, isValidNumberCharacter);
        tokens.push_back(std::make_unique<Token>(TokenType::ImmediateValue, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Assignment Operator
    /// ==========
    else if (c == '=')
    {
        Log::LEXER->trace("Read Assign");
        totalRead++;
        tokens.push_back(std::make_unique<Token>(TokenType::Assignment, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Binary Operators
    /// ==========
    else if (OperatorTypeLookup.find(std::string(1, c)) != OperatorTypeLookup.end())
    {
        Log::LEXER->trace("Read BinaryOperator");
        totalRead++;
        tokens.push_back(std::make_unique<OperatorToken>(data.substr(0, totalRead), currentPosition(totalRead)));
    }

    /// ==========
    /// Comma Seperator
    /// ==========
    else if (c == ',')
    {
        Log::LEXER->trace("Read Comma");
        totalRead++;
        tokens.push_back(std::make_unique<Token>(TokenType::CommaSeperator, data.substr(0, totalRead), currentPosition(totalRead)));
    }

    else
    {
        program.printCurrentTokens();
        Log::LEXER->error("{}", program.Lines[lineIndex]);
        Log::LEXER->error("{: >{}}", '^', charactersReadThisLine + 1);
        Log::LEXER->error("Failed to parse character '{}' (0x{:0x}) Bailing", c, c);
        exit(1);
    }
    charactersReadThisLine += totalRead;
    return totalRead;

}

void LexFile(std::string data, Program& program)
{
    std::vector<std::string> lines;
    std::stringstream ss(data);
    std::string temp;
    while (std::getline(ss, temp, '\n')) {
        lines.push_back(temp);
    }
    std::string_view content = data;
    while (1)
    {
        int read = readToken(content, program.Tokens, program);
        if (read == -1)
        {
            program.printCurrentTokens();
            return;
        }
        Log::LEXER->trace("read {} chars", read);
        content = content.substr(read);
    }
    return;
}
}