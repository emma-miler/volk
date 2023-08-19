#pragma once

#include <vector>
#include <memory>
#include "type.h"

#include "expression.h"
namespace Volk {

class Scope
{
public:
    std::vector<std::unique_ptr<Expression>> Expressions;

public:
    Scope() {}

    void AddBuiltinTypes();
};

}
