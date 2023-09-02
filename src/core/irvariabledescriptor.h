#pragma once

#include <string>

class IRVariableDescriptor
{
public:
    std::string Name;
    std::string Type;
    bool IsPointer;
    bool IsConstant = 0;

    IRVariableDescriptor(std::string name, std::string type, bool isPointer)
    {
        Name = name;
        Type = type;
        IsPointer = isPointer;
    }

    std::string Get()
    {
        std::string builder = "";
        builder += Type;
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
