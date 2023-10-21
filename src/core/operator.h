#pragma once

#include <string>
#include <map>
#include <array>
#include <functional>

#include "irvariabledescriptor.h"
#include "expression_base.h"
#include "type.h"

namespace Volk
{

enum class OperatorType
{
    Null,
	OperatorLogicalOr,
    OperatorLogicalAnd,
    OperatorBitwiseOr,
    OperatorBitwiseAnd,
	
    OperatorEq,
    OperatorNe,
    OperatorGt,
    OperatorGe,
    OperatorLt,
    OperatorLe,		
	
	OperatorPlus,
    OperatorMinus,
    OperatorMultiply,
    OperatorDivide,
	OperatorModulo,
	
};

constexpr std::array<Volk::OperatorType, 2> UnaryOperators =
{
    OperatorType::OperatorPlus,
    OperatorType::OperatorMinus
};


static std::map<OperatorType, std::string> OperatorTypeNames =
{
    {OperatorType::Null, "Null"},    
	
    {OperatorType::OperatorLogicalOr, "OperatorLogicalOr"},
    {OperatorType::OperatorLogicalAnd, "OperatorLogicalAnd"},
    {OperatorType::OperatorBitwiseOr, "OperatorBitwiseOr"},
    {OperatorType::OperatorBitwiseAnd, "OperatorBitwiseAnd"},

    {OperatorType::OperatorPlus, "OperatorPlus"},
    {OperatorType::OperatorMinus, "OperatorMinus"},
    {OperatorType::OperatorMultiply, "OperatorMultiply"},
    {OperatorType::OperatorDivide, "OperatorDivide"},
    {OperatorType::OperatorModulo, "OperatorModulo"},
	
    {OperatorType::OperatorEq, "OperatorEQ"},
    {OperatorType::OperatorNe, "OperatorNE"},
    {OperatorType::OperatorGt, "OperatorGT"},
    {OperatorType::OperatorGe, "OperatorGE"},
    {OperatorType::OperatorLt, "OperatorLT"},
    {OperatorType::OperatorLe, "OperatorLE"},
};

static std::map<std::string, OperatorType> OperatorTypeLookup =
{
    {"+", OperatorType::OperatorPlus},
    {"-", OperatorType::OperatorMinus},
    {"*", OperatorType::OperatorMultiply},
    {"/", OperatorType::OperatorDivide},
    {"%", OperatorType::OperatorModulo},
    {"&", OperatorType::OperatorBitwiseAnd},
    {"|", OperatorType::OperatorBitwiseOr},
};


static std::map<OperatorType, std::string> OperatorInstructionLookup =
{
    {OperatorType::Null, "null"},
    {OperatorType::OperatorPlus, "add"},
    {OperatorType::OperatorMinus, "sub"},
    {OperatorType::OperatorMultiply, "mul"},
    {OperatorType::OperatorDivide, "div"},
    {OperatorType::OperatorModulo, "rem"},
    {OperatorType::OperatorEq, "eq"},
    {OperatorType::OperatorNe, "ne"},
    {OperatorType::OperatorGt, "gt"},
    {OperatorType::OperatorGe, "ge"},
    {OperatorType::OperatorLt, "lt"},
    {OperatorType::OperatorLe, "le"},
};

typedef std::function<void(ExpressionStack&, OperatorType, IRVariableDescriptor, IRVariableDescriptor)> ComparisonFunction;
ComparisonFunction GetComparisonFunction(std::shared_ptr<VKType> type);

}
