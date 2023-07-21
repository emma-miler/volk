#pragma once

#include <string>
#include <spdlog/spdlog.h>

namespace Volk::Log
{
    extern std::shared_ptr<spdlog::logger> FRONTEND;
    extern std::shared_ptr<spdlog::logger> LEXER;
}

void InitializeLogging();
