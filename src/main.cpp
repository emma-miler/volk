#include <iostream>
#include <fstream>

#include "parsing/lexer.h"
#include "parsing/parser.h"
#include "spdlog/spdlog.h"
#include "log/log.h"
#include "llvm/llvm.h"

#include "core/program.h"

#include "util/options.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/sink.h>

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::trace);
    InitializeLogging();
    spdlog::info("starting");

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
        if (arg == "-ftrace-lexer")
        {
            FF_TRACE_LEXER = 1;
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

    Volk::Program program;

    if (FF_TRACE_LEXER)
        Volk::Log::LEXER->set_level(spdlog::level::trace);
    else
        Volk::Log::LEXER->set_level(spdlog::level::debug);

    Volk::LexFile(content, program);

    Volk::VKParser parser { &program };

    while (program.Tokens.size() > 0)
    {
        parser.parse();
    }
    program.printStringTable();
    program.printExpressionTree();


    for (auto&& scope : program.Scopes)
    {
        parser.parserPass_NameResolution(scope.get());
    }

    Volk::Log::FRONTEND->debug("Name resolution:");
    for (auto&& expr : program.DefaultScope->Expressions)
    {
        Volk::Log::FRONTEND->debug("\n" + expr->ToHumanReadableString(""));
    }

    for (auto&& scope : program.Scopes)
    {
        parser.parserPass_TypeChecking(scope.get());

    }
    Volk::Log::FRONTEND->debug("");
    Volk::Log::FRONTEND->debug("");
    Volk::Log::FRONTEND->debug("");
    Volk::Log::FRONTEND->debug("");
    Volk::Log::FRONTEND->debug("Type checking:");
    for (auto&& expr : program.DefaultScope->Expressions)
    {
        Volk::Log::FRONTEND->debug("\n" + expr->ToHumanReadableString(""));
    }

    for (auto&& func : program.DefaultScope->Functions)
    {
        Volk::Log::FRONTEND->debug("Function: '{}'", func.first);
        Volk::Log::FRONTEND->debug(func.second->ToHumanReadable());
        for (auto&& expr : func.second->FunctionScope->Expressions)
        {
            Volk::Log::FRONTEND->debug("\n" + expr->ToHumanReadableString(""));
        }
    }

    std::ofstream output ("scripts/sample.ll");
    output << llvm.generateOutput(program);
    output.close();

    return 0;
}
