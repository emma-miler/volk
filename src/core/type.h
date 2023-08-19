#pragma once

#include "object.h"

namespace Volk
{

class Type : public Object
{
    bool IsReferenceType;
    unsigned int Size;
    std::string LLVMType;
public:
    Type(std::string name, unsigned int size) : Object(name)
    {
        Size = size;
        IsReferenceType = 1;
    }

    Type(std::string name, unsigned int size, std::string llvmType) : Object(name)
    {
        Size = size;
        LLVMType = llvmType;
        IsReferenceType = 0;
    }
};

}
