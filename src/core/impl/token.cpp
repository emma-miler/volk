#include "../token.h"
#include "../program.h"

namespace Volk
{
std::string Token::ToString()
{
    return fmt::format("{}(value='{}')", TokenTypeNames[Type], Value);
}
void Token::Indicate()
{
    Log::PARSER->info("{}", Position.Source->Lines[Position.LineIndex]);
    std::string space = "";
    if (Position.LineOffset > 1)
    {
        space = fmt::format("{: >{}}", ' ', Position.LineOffset - 1);
    }
    std::string tokenIndicator = fmt::format("{:^>{}}", ' ', Position.Length + 1);
    Log::PARSER->info(space + tokenIndicator);
    // +1 because source lines are counted from 1
    Log::PARSER->info("Line: {}, Offset {}", Position.LineIndex + 1, Position.LineOffset);
}
}
