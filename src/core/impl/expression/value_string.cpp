#include "../../expression/value_string.h"

namespace Volk
{

std::string StringConstantValueExpression::ToHumanReadableString(std::string depthPrefix)
{
    return fmt::format("StringConstantValueExpression(index={})", Index);
}

void StringConstantValueExpression::ToIR(ExpressionStack& stack)
{
    // We dont want to actually advance the counter here,
    // since we're not actually pushing anything new
    stack.AdvanceActive(1);
    stack.ActiveVariable.IsConstant = 1;
    stack.ActiveVariable.Name = fmt::format(".str.{}", Index);
    stack.NameCounter--;
    stack.Comment("PUSHED STRING CONSTANT VALUE");
}
}
