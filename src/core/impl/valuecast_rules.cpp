#include "../valuecast_rules.h"

void deref_pointer_if_present(Volk::ExpressionStack& stack, std::string type)
{
	if (stack.ActiveVariable.IsPointer)
    {
		IRVariableDescriptor value = stack.ActiveVariable;
		stack.AdvanceActive(false);
		stack.ActiveVariable.Type = type;
        stack.Operation(fmt::format("{} = load {}, ptr {}", stack.ActiveVariable.GetOnlyName(), type, value.GetOnlyName()));
    }
}

void int_to_float(Volk::ExpressionStack& stack)
{
	deref_pointer_if_present(stack, "i64");
	IRVariableDescriptor value = stack.ActiveVariable;
	stack.AdvanceActive(false);
	stack.ActiveVariable.Type = "float";
	stack.Operation(fmt::format("{} = sitofp {} to float", stack.ActiveVariable.GetOnlyName(), value.Get()));
}
void int_to_double(Volk::ExpressionStack& stack)
{
	deref_pointer_if_present(stack, "i64");
	IRVariableDescriptor value = stack.ActiveVariable;
	stack.AdvanceActive(false);
	stack.ActiveVariable.Type = "double";
	stack.Operation(fmt::format("{} = sitofp {} to double", stack.ActiveVariable.GetOnlyName(), value.Get()));
}

void float_to_int(Volk::ExpressionStack& stack)
{
	deref_pointer_if_present(stack, "float");
	IRVariableDescriptor value = stack.ActiveVariable;
	stack.AdvanceActive(false);
	stack.ActiveVariable.Type = "i64";
	stack.Operation(fmt::format("{} = fptosi {} to i64", stack.ActiveVariable.GetOnlyName(), value.Get()));
}

void float_to_double(Volk::ExpressionStack& stack)
{
	deref_pointer_if_present(stack, "float");
	IRVariableDescriptor value = stack.ActiveVariable;
	stack.AdvanceActive(false);
	stack.ActiveVariable.Type = "double";
	stack.Operation(fmt::format("{} = fpext {} to double", stack.ActiveVariable.GetOnlyName(), value.Get()));
}

void double_to_float(Volk::ExpressionStack& stack)
{
	deref_pointer_if_present(stack, "double");
	IRVariableDescriptor value = stack.ActiveVariable;
	stack.AdvanceActive(false);
	stack.ActiveVariable.Type = "float";
	stack.Operation(fmt::format("{} = fptrunc {} to float", stack.ActiveVariable.GetOnlyName(), value.Get()));
}

void double_to_int(Volk::ExpressionStack& stack)
{
	deref_pointer_if_present(stack, "double");
	IRVariableDescriptor value = stack.ActiveVariable;
	stack.AdvanceActive(false);
	stack.ActiveVariable.Type = "i64";
	stack.Operation(fmt::format("{} = fptosi {} to i64", stack.ActiveVariable.GetOnlyName(), value.Get()));
}


namespace Volk
{
std::function<void(ExpressionStack&)> GetSidecastForTypes(std::shared_ptr<VKType> source, std::shared_ptr<VKType> target)
{
	if (source == BUILTIN_INT)
	{
		if (target == BUILTIN_FLOAT) return &int_to_float;
		if (target == BUILTIN_DOUBLE) return &int_to_double;
	}
	if (source == BUILTIN_FLOAT)
	{
		if (target == BUILTIN_INT) return &float_to_int;
		if (target == BUILTIN_DOUBLE) return &float_to_double;
	}
	if (source == BUILTIN_DOUBLE)
	{
		if (target == BUILTIN_FLOAT) return &double_to_float;
		if (target == BUILTIN_INT) return &double_to_int;
	}
	return nullptr;
}
}
