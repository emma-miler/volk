#pragma once

#include <string>

class IRVariableDescriptor
{
public:
    std::string Name;
    bool IsPointer;

    IRVariableDescriptor(std::string name, bool isPointer)
    {
        Name = name;
        IsPointer = isPointer;
    }
};
