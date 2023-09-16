#include "../../expression/value_binary.h"
#include "../../valuecast_rules.h"

namespace Volk
{
std::string BinaryValueExpression::ToHumanReadableString(std::string depthPrefix)
{
    std::string newline = fmt::format("\n{}\t", depthPrefix);
    std::string out = "BinaryOperatorValueExpression(";
    if (ResolvedType != nullptr)
    {
        out += newline + fmt::format("type='{}'", ResolvedType->Name);
    }
    out += newline + fmt::format("op='{}'", OperatorTypeNames[Operator]);
    out += newline + fmt::format("left='{}'", Left->ToHumanReadableString(depthPrefix + "\t"));
    out += newline + fmt::format("right='{}'", Right->ToHumanReadableString(depthPrefix + "\t"));
    out += "\n" + depthPrefix + ")";
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
            stack.Expressions.push_back(fmt::format("%{} = load {}, {}", stack.ActiveVariable.Name, Left->ResolvedType->LLVMType, left.Get()));
            left.Name = stack.ActiveVariable.Name;
            left.IsPointer = 0;
        }
        if (right.IsPointer)
        {
            stack.AdvanceActive(0);
            stack.Expressions.push_back(fmt::format("%{} = load {}, {}", stack.ActiveVariable.Name, Left->ResolvedType->LLVMType, right.Get()));
            right.Name = stack.ActiveVariable.Name;
            right.IsPointer = 0;
        }
        stack.Comment("END BINARY OPERATOR PRELOAD\n");
    }

    // Perform the operator
    stack.AdvanceActive(0);
    stack.Comment("START BINARY OPERATOR");
    stack.Expressions.push_back(fmt::format("%{} = {}{} {} {}, {}", 	stack.ActiveVariable.Name, 
																	Left->ResolvedType == BUILTIN_FLOAT || Left->ResolvedType == BUILTIN_DOUBLE ? "f" : "",
																	OperatorInstructionLookup[Operator],
																	Left->ResolvedType->LLVMType,
																	left.GetOnlyName(),
																	right.GetOnlyName()));
    stack.Comment("END BINARY OPERATOR\n");
}

std::vector<Expression*> BinaryValueExpression::SubExpressions()
{
    return std::vector<Expression*>{ Left.get(), Right.get() };
}

void BinaryValueExpression::TypeCheck(Scope* scope)
{
    if (Left->ResolvedType != Right->ResolvedType)
    {
        Log::TYPESYS->error("No valid operator '{}' between types '{}' and '{}'", OperatorTypeNames[Operator], Left->ResolvedType->Name, Right->ResolvedType->Name);
        Token->Indicate();
        throw type_error("");
    }
    // TODO: actually do this properly
    ResolvedType = Left->ResolvedType;
}
}
