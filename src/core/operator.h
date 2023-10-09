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
    OperatorPlus,
    OperatorMinus,
    OperatorMultiply,
    OperatorDivide,
    OperatorEq,
    OperatorNeq,
    OperatorGt,
    OperatorGte,
    OperatorLt,
    OperatorLte
};

constexpr std::array<Volk::OperatorType, 2> UnaryOperators =
{
    OperatorType::OperatorPlus,
    OperatorType::OperatorMinus
};


static std::map<OperatorType, std::string> OperatorTypeNames =
{
    {OperatorType::Null, "Null"},
    {OperatorType::OperatorPlus, "OperatorPlus"},
    {OperatorType::OperatorMinus, "OperatorMinus"},
    {OperatorType::OperatorMultiply, "OperatorMultiply"},
    {OperatorType::OperatorDivide, "OperatorDivide"},
    {OperatorType::OperatorEq, "OperatorEQ"},
    {OperatorType::OperatorNeq, "OperatorNEQ"},
    {OperatorType::OperatorGt, "OperatorGT"},
    {OperatorType::OperatorGte, "OperatorGTE"},
    {OperatorType::OperatorLt, "OperatorLT"},
    {OperatorType::OperatorLte, "OperatorLTE"},
};

static std::map<std::string, OperatorType> OperatorTypeLookup =
{
    {"+", OperatorType::OperatorPlus},
    {"-", OperatorType::OperatorMinus},
    {"*", OperatorType::OperatorMultiply},
    {"/", OperatorType::OperatorDivide},
};


static std::map<OperatorType, std::string> OperatorInstructionLookup =
{
    {OperatorType::Null, "null"},
    {OperatorType::OperatorPlus, "add"},
    {OperatorType::OperatorMinus, "sub"},
    {OperatorType::OperatorMultiply, "mul"},
    {OperatorType::OperatorDivide, "div"},
    {OperatorType::OperatorEq, "eq"},
    {OperatorType::OperatorNeq, "neq"},
    {OperatorType::OperatorGt, "gt"},
    {OperatorType::OperatorGte, "ge"},
    {OperatorType::OperatorLt, "lt"},
    {OperatorType::OperatorLte, "le"},

};

typedef std::function<void(ExpressionStack&, OperatorType, IRVariableDescriptor, IRVariableDescriptor)> ComparisonFunction;
ComparisonFunction GetComparisonFunction(std::shared_ptr<VKType> type);

}
