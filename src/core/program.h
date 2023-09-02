#pragma once

#include "token.h"
#include "expression.h"
#include "scope.h"
#include "namespace.h"

#include <deque>

namespace Volk
{
class Program
{
public:
    std::deque<std::shared_ptr<Token>> Tokens;
    std::vector<std::unique_ptr<Expression>> Expressions;
    std::vector<std::string> Lines;

    std::unique_ptr<Namespace> RootNamespace;
    std::shared_ptr<Scope> DefaultScope;

    std::vector<std::shared_ptr<Scope>> Scopes;
    std::deque<std::shared_ptr<Scope>> ActiveScopes;

    std::vector<std::string> StringTable;


    Program()
    {
        RootNamespace = std::make_unique<Namespace>("");
        DefaultScope = std::make_shared<Scope>(nullptr);
        Scopes.push_back(DefaultScope);
        ActiveScopes.push_front(DefaultScope);
        DefaultScope->AddBuiltinTypes();
    }

    void printCurrentTokens();
    void printStringTable();
    void printExpressionTree();

    std::shared_ptr<Type> FindType(std::string typeName);
};

}
