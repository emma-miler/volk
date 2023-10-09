#pragma once
#include <map>
#include <string>
#include <functional>
#include <memory>
#include "type.h"
#include "expression_base.h"

namespace Volk
{
std::function<void(ExpressionStack&)> GetSidecastForTypes(std::shared_ptr<VKType>, std::shared_ptr<VKType>);
}