#pragma once

#include <vector>
#include <memory>

#include "expression.h"
namespace Volk {

class Scope
{
public:
    std::vector<std::unique_ptr<Expression>> Expressions;

public:
    Scope() {}
};

}
