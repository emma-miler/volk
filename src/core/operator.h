#pragma once

#include <string>
#include <map>
#include <array>

namespace Volk
{

enum class OperatorType
{
    OperatorPlus,
    OperatorMinus,
    OperatorMultiply,
    OperatorDivide
};

constexpr std::array<Volk::OperatorType, 2> UnaryOperators =
{
    OperatorType::OperatorPlus,
    OperatorType::OperatorMinus
};


static std::map<OperatorType, std::string> OperatorTypeNames =
{
    {OperatorType::OperatorPlus, "OperatorPlus"},
    {OperatorType::OperatorMinus, "OperatorMinus"},
    {OperatorType::OperatorMultiply, "OperatorMultiply"},
    {OperatorType::OperatorDivide, "OperatorDivide"},
};

static std::map<std::string, OperatorType> OperatorTypeLookup =
{
    {"+", OperatorType::OperatorPlus},
    {"-", OperatorType::OperatorMinus},
    {"*", OperatorType::OperatorMultiply},
    {"/", OperatorType::OperatorDivide},
};

}
