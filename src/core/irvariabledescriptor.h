#pragma once

#include <string>

enum class IRVarType
{
	Immediate = 0,
	Constant = 1,
	Variable = 2,
	Pointer = 3
};

class IRVariableDescriptor
{
public:
    std::string Name;
    std::string Type;
	bool IsPointer;
    IRVarType VarType;

    IRVariableDescriptor(std::string name, std::string type, bool isPointer, IRVarType varType)
    {
        Name = name;
        Type = type;
		IsPointer = isPointer;
		VarType = varType;
    }

    std::string Get()
    {
        std::string builder = "";
        builder += Type;
        builder += " ";
		switch (VarType)
		{
			case IRVarType::Immediate: break;
			case IRVarType::Constant: 
				builder += '@'; break;
			case IRVarType::Variable: 
			case IRVarType::Pointer:
				builder += '%'; break;
			default: break;
		}
        builder += Name;
        return builder;
    }

    std::string GetOnlyName()
    {
        std::string builder = "";
        switch (VarType)
		{
			case IRVarType::Immediate: break;
			case IRVarType::Constant: 
				builder += '@'; break;
			case IRVarType::Variable: 
			case IRVarType::Pointer:
				builder += '%'; break;
			default: break;
		}
        builder += Name;
        return builder;
    }
};
