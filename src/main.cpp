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



    Volk::LexFile(content, program);

    Volk::VKParser parser { &program };

    while (program.Tokens.size() > 0)
    {
        parser.parse();
    }
    program.printStringTable();
    program.printExpressionTree();

    for (auto&& expr : program.DefaultScope->Expressions)
    {
        parser.visitExpression(expr.get(), program.DefaultScope.get());
        Volk::Log::FRONTEND->debug("\n" + expr->ToHumanReadableString(""));
    }

    for (auto&& func : program.RootNamespace->Functions)
    {
        Volk::Log::FRONTEND->debug("Function: '{}'", func->Name);
        Volk::Log::FRONTEND->debug(func->ToHumanReadable());
        for (auto&& expr : func->FunctionScope->Expressions)
        {
            Volk::Log::FRONTEND->debug("\n" + expr->ToHumanReadableString(""));
        }
    }

    std::ofstream output ("scripts/sample.ll");
    output << llvm.generateOutput(program);
    output.close();

    return 0;
}
