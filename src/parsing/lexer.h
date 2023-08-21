#pragma once

#include <vector>
#include "../core/token.h"
#include "../core/expression.h"
#include "../core/program.h"
#include <string_view>
#include <functional>
#include <deque>
#include <memory>
#include <optional>

#include "../core/namespace.h"

namespace Volk
{
void LexFile(std::string data, Program& program);
}
