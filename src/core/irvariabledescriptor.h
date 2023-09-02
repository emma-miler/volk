#pragma once

#include <string>

class IRVariableDescriptor
{
public:
    std::string Name;
    bool IsPointer;
    bool IsConstant = 0;

    IRVariableDescriptor(std::string name, bool isPointer)
    {
        Name = name;
        IsPointer = isPointer;
    }

    std::string Get()
    {
        std::string builder = "";
        builder += IsPointer ? "ptr" : "i64";
        builder += " ";
        builder += IsConstant ? "@" : "%";
        builder += Name;
        return builder;
    }

    std::string GetOnlyName()
    {
        std::string builder = "";
        builder += IsConstant ? "@" : "%";
        builder += Name;
        return builder;
    }
};
