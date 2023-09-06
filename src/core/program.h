#pragma once

#include "scope.h"

#include <deque>

namespace Volk
{
    class Program;
}

#include "token.h"
#include "function.h"
#include "source.h"

namespace Volk
{
class Program
{
public:
    std::deque<std::shared_ptr<Token>> Tokens;
    std::shared_ptr<ProgramSource> Source;

    std::shared_ptr<Scope> DefaultScope;

    std::vector<std::shared_ptr<Scope>> Scopes;
    std::deque<std::shared_ptr<Scope>> ActiveScopes;

    std::vector<std::string> StringTable;


    Program();

    void printCurrentTokens();
    void printStringTable();
    void printExpressionTree();

    std::shared_ptr<VKType> FindType(std::string typeName);
};

}
