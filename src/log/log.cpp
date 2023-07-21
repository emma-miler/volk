
#include "log.h"
#include "../util/color.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <string_view>

namespace Volk::Log
{
	std::shared_ptr<spdlog::logger> FRONTEND;
	std::shared_ptr<spdlog::logger> LEXER;
}; // namespace Volk::log

std::shared_ptr<spdlog::logger> createLogger(std::string name, Color color)
{
// return "[" + color.ToANSIColor() + name + "\033[39;49m]";

    auto logger = std::make_shared<spdlog::logger>(color.ToANSIColor() + "[" + name + "]" + Color::ANSIResetCode);
    logger->set_level(spdlog::default_logger()->level());
    for (auto&& sink : spdlog::default_logger()->sinks())
    {
        logger->sinks().push_back(sink);
    }
    return logger;
}

void InitializeLogging()
{
    spdlog::set_pattern("[%H:%M:%S.%e]%^[%l]%$%n %v");
    Volk::Log::FRONTEND = createLogger("Main", Volk::Colors::Main);
    Volk::Log::LEXER = createLogger("Lexer", Volk::Colors::Lexer);
}
