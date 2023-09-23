#include "../operator.h"

namespace Volk
{

void compare_int(ExpressionStack& stack, OperatorType op, IRVariableDescriptor left, IRVariableDescriptor right)
{
    std::string opString = OperatorInstructionLookup[op];
    if (op == OperatorType::OperatorGt || op == OperatorType::OperatorGte || op == OperatorType::OperatorLt || op==OperatorType::OperatorLte)
    {
        opString = "s" + opString;
    }
    stack.Operation(fmt::format("{} = icmp {} i64 {}, {}", stack.ActiveVariable.GetOnlyName(),
                                                           opString,
                                                           left.GetOnlyName(),
                                                           right.GetOnlyName()
                               ));
    stack.ActiveVariable.Type = "i1";
}

ComparisonFunction GetComparisonFunction(std::shared_ptr<VKType> type)
{
    if (type == BUILTIN_INT)
        return compare_int;
    return nullptr;
}
}
