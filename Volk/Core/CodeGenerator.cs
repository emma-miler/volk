using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core.Objects;

namespace Volk.Core;
public class CodeGenerator
{

    List<string> _lines = new();
    public int Counter { get; set; } = 0;
    public string LastJumpPoint { get; private set; } = string.Empty;

    public void AddStringTable(List<VKCompileTimeString> stringObjects)
    {
         foreach (VKCompileTimeString str in stringObjects)
        {
            _lines.Add($"@.str.{str.Index} = private unnamed_addr constant [{str.Value.Length + 1} x i8] c\"{str.AsLLVMString() + "\\00"}\", align 1");
        }
    }

    public IRVariable NewVariable(VKType type, IRVariableType variableType)
    {
        IRVariable var = new IRVariable(Counter.ToString(), type, variableType);
        Counter++;
        return var;
    }

    public string Build()
    {
        return string.Join('\n', _lines);
    }

    public void Comment(string comment)
    {
        _lines.Add($"\t; {comment}");
    }

    public void Operation(string operation)
    {
        _lines.Add($"\t{operation}");
    }

    public void Label(string label)
    {
        _lines.Add(label);
    }

    public void Jump(string label, bool updateLastJumpPoint = true)
    {
        _lines.Add($"\br label %{label}");
        if (updateLastJumpPoint)
            LastJumpPoint = label;
    }

    public void Branch(IRVariable condition, string labelIfTrue, string labelIfFalse)
    {
        _lines.Add($"\br i1 {condition.GetName()}, label %{labelIfTrue}, label %{labelIfFalse}");
    }
}