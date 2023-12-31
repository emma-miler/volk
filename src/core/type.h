#pragma once

#include <memory>
#include <functional>
#include <map>
#include "object.h"

namespace Volk
{
    class VKType;
}

#include "expression_base.h"


namespace Volk
{
class VKType : public Object
{
public:
    bool IsReferenceType;
    unsigned int Size;
    std::string LLVMType;

    std::map<std::shared_ptr<VKType>, std::function<void(ExpressionStack&)>> ImplicitConverters;

    VKType(std::string name, unsigned int size) : Object(name)
    {
        Size = size;
        IsReferenceType = 1;
    }

    VKType(std::string name, unsigned int size, std::string llvmType) : Object(name)
    {
        Size = size;
        LLVMType = llvmType;
        IsReferenceType = 0;
    }

    virtual std::string ToString()
    {
        return Name;
    }

    virtual ~VKType() = default;
};

extern std::shared_ptr<VKType> BUILTIN_INT;
extern std::shared_ptr<VKType> BUILTIN_FLOAT;
extern std::shared_ptr<VKType> BUILTIN_DOUBLE;
extern std::shared_ptr<VKType> BUILTIN_BOOL;
extern std::shared_ptr<VKType> BUILTIN_STRING;
extern std::shared_ptr<VKType> BUILTIN_FUNCTION;
extern std::shared_ptr<VKType> BUILTIN_VOID;
extern std::shared_ptr<VKType> BUILTIN_VARARGS;
}
