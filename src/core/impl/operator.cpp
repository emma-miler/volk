#include "../operator.h"

namespace Volk
{

void compare_int(ExpressionStack& stack, OperatorType op, IRVariableDescriptor left, IRVariableDescriptor right)
{
    std::string opString = OperatorInstructionLookup[op];
    if (op == OperatorType::OperatorGt || op == OperatorType::OperatorGe || op == OperatorType::OperatorLt || op==OperatorType::OperatorLe)
    {
        opString = "s" + opString;
    }
    stack.Operation("{} = icmp {} i64 {}, {}", stack.ActiveVariable.GetOnlyName(),
                                                           opString,
                                                           left.GetOnlyName(),
                                                           right.GetOnlyName()
                               );
    stack.ActiveVariable.Type = "i1";
}

void compare_float(ExpressionStack& stack, OperatorType op, IRVariableDescriptor left, IRVariableDescriptor right)
{
    std::string opString = OperatorInstructionLookup[op];
    if (op == OperatorType::OperatorGt || op == OperatorType::OperatorGe || op == OperatorType::OperatorLt || op==OperatorType::OperatorLe)
    {
        opString = "o" + opString;
    }
    stack.Operation("{} = fcmp {} float {}, {}", stack.ActiveVariable.GetOnlyName(),
                                                           opString,
                                                           left.GetOnlyName(),
                                                           right.GetOnlyName()
                               );
    stack.ActiveVariable.Type = "i1";
}

void compare_double(ExpressionStack& stack, OperatorType op, IRVariableDescriptor left, IRVariableDescriptor right)
{
    std::string opString = OperatorInstructionLookup[op];
    if (op == OperatorType::OperatorGt || op == OperatorType::OperatorGe || op == OperatorType::OperatorLt || op==OperatorType::OperatorLe)
    {
        opString = "o" + opString;
    }
    stack.Operation("{} = fcmp {} double {}, {}", stack.ActiveVariable.GetOnlyName(),
                                                           opString,
                                                           left.GetOnlyName(),
                                                           right.GetOnlyName()
                               );
    stack.ActiveVariable.Type = "i1";
}

void compare_bool(ExpressionStack& stack, OperatorType op, IRVariableDescriptor left, IRVariableDescriptor right)
{
    std::string opString = OperatorInstructionLookup[op];
    if (op == OperatorType::OperatorGt || op == OperatorType::OperatorGe || op == OperatorType::OperatorLt || op==OperatorType::OperatorLe)
    {
        Log::PARSER->critical("Ordering operators (>,>=,<,<=) cannot be used on type 'bool'");
		throw new std::format_error("");
    }
    stack.Operation("{} = icmp {} i1 {}, {}", stack.ActiveVariable.GetOnlyName(),
                                                           opString,
                                                           left.GetOnlyName(),
                                                           right.GetOnlyName()
                               );
    stack.ActiveVariable.Type = "i1";
}

ComparisonFunction GetComparisonFunction(std::shared_ptr<VKType> type)
{
    if (type == BUILTIN_INT)
        return compare_int;
    if (type == BUILTIN_FLOAT)
        return compare_float;
    if (type == BUILTIN_DOUBLE)
        return compare_double;
	if (type == BUILTIN_BOOL)
		return compare_bool;
    Log::TYPESYS->error("No valid comparison function for type '{}'", type->ToString());
    return nullptr;
}
}
