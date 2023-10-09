#include "../../expression/value_binary.h"
#include "../../expression/value_valuecast.h"
#include "../../valuecast_rules.h"

namespace Volk
{
std::string BinaryValueExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}{}", depthPrefix, INDENT);
    std::string out = "BinaryOperatorValueExpression";
    if (ResolvedType != nullptr)
    {
        out += newline + fmt::format("type={}", ResolvedType->Name);
    }
    out += newline + fmt::format("op={}", OperatorTypeNames[Operator]);
    out += newline + fmt::format("left={}", Left->ToHumanReadableString(depthPrefix + INDENT));
    out += newline + fmt::format("right={}", Right->ToHumanReadableString(depthPrefix + INDENT));
    return  out;
}

void BinaryValueExpression::ToIR(ExpressionStack& stack)
{
    Left->ToIR(stack);
    IRVariableDescriptor left = stack.ActiveVariable;

    Right->ToIR(stack);
    IRVariableDescriptor right = stack.ActiveVariable;

    // LLVM does not allow you to operate on an immediate i64 with a ptr value or vice versa,
    // and requires that the type of both operands is the same,
    // so we must load the left and/or right side into a temp variable
    if (left.IsPointer || right.IsPointer)
    {
        stack.Comment("START BINARY OPERATOR PRELOAD");
        if (left.IsPointer)
        {
            stack.AdvanceActive(0);
            stack.Operation(fmt::format("%{} = load {}, {}", stack.ActiveVariable.Name, Left->ResolvedType->LLVMType, left.Get()));
            left.Name = stack.ActiveVariable.Name;
            left.IsPointer = 0;
        }
        if (right.IsPointer)
        {
            stack.AdvanceActive(0);
            stack.Operation(fmt::format("%{} = load {}, {}", stack.ActiveVariable.Name, Left->ResolvedType->LLVMType, right.Get()));
            right.Name = stack.ActiveVariable.Name;
            right.IsPointer = 0;
        }
        stack.Comment("END BINARY OPERATOR PRELOAD\n");
    }

    // Perform the operator
    stack.AdvanceActive(0);
	stack.ActiveVariable.Type = Left->ResolvedType->LLVMType;
    stack.Comment("START BINARY OPERATOR");
    if (IsComparator)
    {
        CompareFunction(stack, Operator, left, right);
    }
    else
    {
        stack.Operation(fmt::format("%{} = {}{} {} {}, {}", stack.ActiveVariable.Name,
                                                            Left->ResolvedType == BUILTIN_FLOAT || Left->ResolvedType == BUILTIN_DOUBLE ? "f" : Operator == OperatorType::OperatorDivide ? "s" : "",
                                                            OperatorInstructionLookup[Operator],
                                                            Left->ResolvedType->LLVMType,
                                                            left.GetOnlyName(),
                                                            right.GetOnlyName()));
    }
    stack.Comment("END BINARY OPERATOR\n");
}

std::vector<Expression*> BinaryValueExpression::SubExpressions()
{
    return std::vector<Expression*>{ Left.get(), Right.get() };
}

void BinaryValueExpression::TypeCheck(Scope* scope)
{
	if (Operator == OperatorType::OperatorDivide)
	{
		if (Left->ResolvedType == BUILTIN_INT)
		{
			Left = std::make_unique<ValueCastExpression>(std::move(Left), BUILTIN_DOUBLE, BUILTIN_INT->ImplicitConverters[BUILTIN_DOUBLE]);
		}
		if (Right->ResolvedType == BUILTIN_INT)
		{
			Right = std::make_unique<ValueCastExpression>(std::move(Right), BUILTIN_DOUBLE, BUILTIN_INT->ImplicitConverters[BUILTIN_DOUBLE]);
		}
	}
	// This mess basically makes it so that you can e.g multiple a float by a double,
	// or an int by a float
	// Its a bit ugly, but i dont really know a more "elegant" way of doing it
	if (Left->ResolvedType != Right->ResolvedType)
	{
		if (Left->ResolvedType == BUILTIN_FLOAT && Right->ResolvedType == BUILTIN_INT)
		{
			Right = std::make_unique<ValueCastExpression>(std::move(Right), BUILTIN_FLOAT, BUILTIN_INT->ImplicitConverters[BUILTIN_FLOAT]);
		}
		else if (Left->ResolvedType == BUILTIN_DOUBLE && Right->ResolvedType == BUILTIN_INT)
		{
			Right = std::make_unique<ValueCastExpression>(std::move(Right), BUILTIN_DOUBLE, BUILTIN_INT->ImplicitConverters[BUILTIN_DOUBLE]);
		}
		
		else if (Left->ResolvedType == BUILTIN_INT && Right->ResolvedType == BUILTIN_FLOAT)
		{
			Left = std::make_unique<ValueCastExpression>(std::move(Left), BUILTIN_FLOAT, BUILTIN_INT->ImplicitConverters[BUILTIN_FLOAT]);
		}
		else if (Left->ResolvedType == BUILTIN_INT && Right->ResolvedType == BUILTIN_DOUBLE)
		{
			Left = std::make_unique<ValueCastExpression>(std::move(Left), BUILTIN_DOUBLE, BUILTIN_INT->ImplicitConverters[BUILTIN_DOUBLE]);
		}
		
		else if (Left->ResolvedType == BUILTIN_FLOAT && Right->ResolvedType == BUILTIN_DOUBLE)
		{
			Left = std::make_unique<ValueCastExpression>(std::move(Left), BUILTIN_DOUBLE, BUILTIN_FLOAT->ImplicitConverters[BUILTIN_DOUBLE]);
		}
		else if (Left->ResolvedType == BUILTIN_DOUBLE && Right->ResolvedType == BUILTIN_FLOAT)
		{
			Right = std::make_unique<ValueCastExpression>(std::move(Right), BUILTIN_DOUBLE, BUILTIN_FLOAT->ImplicitConverters[BUILTIN_DOUBLE]);
		}
	}
	if (Left->ResolvedType != Right->ResolvedType)
    {
		Log::TYPESYS->error("{}", *(void**)&Left->ResolvedType);
		Log::TYPESYS->error("{}", *(void**)&Right->ResolvedType);
        Log::TYPESYS->error("No valid operator '{}' between types '{}' and '{}'", OperatorTypeNames[Operator], Left->ResolvedType->Name, Right->ResolvedType->Name);
        Token->Indicate();
        throw type_error("");
    }
    // TODO: actually do this properly
    ResolvedType = Left->ResolvedType;
    if (IsComparator)
    {
        ResolvedType = BUILTIN_BOOL;
        CompareFunction = GetComparisonFunction(Left->ResolvedType);
        if (CompareFunction == nullptr)
        {
            Log::TYPESYS->error("No comparison function for type '{}', between types '{}' and '{}'", Left->ResolvedType->Name, Left->ResolvedType->Name, Right->ResolvedType->Name);
            Token->Indicate();
            throw type_error("");
        }
    }
}
}
