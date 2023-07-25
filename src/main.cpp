#include <iostream>
#include <fstream>

#include "parsing/parser.h"
#include "spdlog/spdlog.h"
#include "log/log.h"
#include "llvm/llvm.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/sink.h>

int main(int argc, char **argv)
{
    spdlog::set_level(spdlog::level::debug);
    InitializeLogging();
    spdlog::info("starting");

    Volk::VKParser parser{};
    Volk::VKLLVM llvm{};
    std::string line;
    std::ifstream script ("scripts/sample.vk");



    if (!script.is_open())
    {
        std::cout << "Unable to open file";
    }

    std::string content( (std::istreambuf_iterator<char>(script) ),
                    (std::istreambuf_iterator<char>()    ) );

    std::cout << "\n" << std::endl;
    std::cout << content << std::endl;
    std::cout << "\n" << std::endl;

    parser.consume(content);
    while (parser.Tokens.size() > 0)
    {
        parser.parse();
    }
    parser.printExpressionTree();

    for (auto&& expr : parser.Expressions)
    {
        Volk::Log::FRONTEND->debug("\n" + expr->ToHumanReadableString("\t"));
    }

    std::ofstream output ("scripts/sample.ll");
    output << llvm.generateOutput(parser);
    output.close();

    return 0;
}
