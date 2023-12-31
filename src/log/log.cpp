
#include "log.h"
#include "../util/color.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/fmt.h>

#include <string_view>

namespace Volk::Log
{
	std::shared_ptr<spdlog::logger> FRONTEND;
	std::shared_ptr<spdlog::logger> LEXER;
	std::shared_ptr<spdlog::logger> PARSER;
	std::shared_ptr<spdlog::logger> TYPESYS;
	std::shared_ptr<spdlog::logger> OUTPUT;
}; // namespace Volk::log

std::shared_ptr<spdlog::logger> createLogger(std::string name, Color color)
{
// return "[" + color.ToANSIColor() + name + "\033[39;49m]";

    auto logger = std::make_shared<spdlog::logger>(color.ToANSIColor() + "[" + name + "]" + ANSIResetCode);
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
    Volk::Log::PARSER = createLogger("Parser", Volk::Colors::Parser);
    Volk::Log::TYPESYS = createLogger("TypeSystem", Volk::Colors::TypeSystem);
    Volk::Log::OUTPUT = createLogger("Output", Volk::Colors::Output);
}
