using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Threading.Tasks;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core;

public enum IRVariableType
{
    Immediate = 0,
    Constant = 1,
    Variable = 2,
}

public record struct IRVariable
{
    public string Name;
    public VKType Type;
    public string IRType => Type.IRType! + '*'.Repeat(PointerDepth);
    public VKType? PointingType;
    public IRVariableType VariableType;

    public int PointerDepth;

    public IRVariable(string name, VKType type, IRVariableType variableType, int pointerDepth = 0)
    {
        Name = name;
        VariableType = variableType;
        Type = type;
        PointingType = null;
        PointerDepth = pointerDepth;
    }

    public string Reference 
    { get {
            string prefix;
            switch (VariableType)
            {
                case IRVariableType.Immediate: prefix = ""; break;
                case IRVariableType.Constant: prefix = "@"; break;
                case IRVariableType.Variable: prefix = "%"; break;
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
            default: throw new InvalidEnumArgumentException();
        }
        return $"{IRType} {prefix}{Name}";
    }
}