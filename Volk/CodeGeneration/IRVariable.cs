using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core.Objects;

namespace Volk.Core;

public enum IRVariableType
{
    Immediate = 0,
    Constant = 1,
    Variable = 2,
    Pointer = 3,
}

public record struct IRVariable
{
    public string Name;
    public VKType Type;
    public IRVariableType VariableType;

    public IRVariable(string name, VKType type, IRVariableType variableType)
    {
        Name = name;
        Type = type;
        VariableType = variableType;
    }

    public string Reference 
    { get {
            string prefix;
            switch (VariableType)
            {
                case IRVariableType.Immediate: prefix = ""; break;
                case IRVariableType.Constant: prefix = "@"; break;
                case IRVariableType.Variable: prefix = "%"; break;
                case IRVariableType.Pointer: prefix = "%"; break;
                default: throw new InvalidEnumArgumentException();
            }
            return $"{prefix}{Name}";
        }
    }

    public override string ToString()
    {
        string prefix;
        switch (VariableType)
        {
            case IRVariableType.Immediate: prefix = ""; break;
            case IRVariableType.Constant: prefix = "@"; break;
            case IRVariableType.Variable: prefix = "%"; break;
            case IRVariableType.Pointer: prefix = "%"; break;
            default: throw new InvalidEnumArgumentException();
        }
        return $"{Type.IRType} {prefix}{Name}";
    }
}