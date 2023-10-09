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
void InitBuiltinSidecastRules()
{
	BUILTIN_INT->ImplicitConverters[BUILTIN_FLOAT] = int_to_float;
	BUILTIN_INT->ImplicitConverters[BUILTIN_DOUBLE] = int_to_double;
	BUILTIN_FLOAT->ImplicitConverters[BUILTIN_INT] = float_to_int;
	BUILTIN_FLOAT->ImplicitConverters[BUILTIN_DOUBLE] = float_to_double;
	BUILTIN_DOUBLE->ImplicitConverters[BUILTIN_INT] = double_to_int;
	BUILTIN_DOUBLE->ImplicitConverters[BUILTIN_FLOAT] = double_to_float;
}
}
