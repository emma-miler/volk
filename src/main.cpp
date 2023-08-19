#include <iostream>
#include <fstream>

#include "parsing/parser.h"
#include "spdlog/spdlog.h"
#include "log/log.h"
#include "llvm/llvm.h"

#include "util/options.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/sink.h>

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::trace);
    InitializeLogging();
    spdlog::info("starting");

    Volk::VKParser parser{};
    Volk::VKLLVM llvm{};
    std::string line;
    std::ifstream script ("scripts/sample.vk");

    for (int i = 0; i < argc; i++)
    {
        std::string arg = argv[i];
        Volk::Log::FRONTEND->debug("Arg: '{}'", arg);
        if (arg == "-fno-verbose-ir")
        {
            FF_LLVM_VERBOSE = 0;
        }
    }

    Volk::Log::FRONTEND->info("Feature flag LLVM_VERBOSE: {}", FF_LLVM_VERBOSE);


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
    parser.printStringTable();
    parser.printExpressionTree();

    for (auto&& expr : parser.DefaultScope->Expressions)
    {
        Volk::Log::FRONTEND->debug("\n" + expr->ToHumanReadableString(""));
    }

    for (auto&& func : parser.RootNamespace->Functions)
    {
        Volk::Log::FRONTEND->debug("Function: '{}'", func->Name);
        Volk::Log::FRONTEND->debug(func->ToHumanReadable());
        for (auto&& expr : func->FunctionScope->Expressions)
        {
            Volk::Log::FRONTEND->debug("\n" + expr->ToHumanReadableString(""));
        }
    }

    std::ofstream output ("scripts/sample.ll");
    output << llvm.generateOutput(parser);
    output.close();

    return 0;
}
