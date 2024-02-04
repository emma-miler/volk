using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using Volk.Core.Objects;

namespace Volk.Core;
public class CodeGenerator
{

    List<string> _lines = new();
    public int Counter { get; set; } = 0;
    public string LastJumpPoint { get; set; } = string.Empty;

    public void AddStringTable(List<VKCompileTimeString> stringObjects)
    {
         foreach (VKCompileTimeString str in stringObjects)
        {
            _lines.Add($"@.str.{str.Index} = private unnamed_addr constant [{str.Value.Length + 1} x i8] c\"{str.AsLLVMString() + "\\00"}\", align 1");
        }
    }

    public IRVariable NewVariable(VKType type, IRVariableType variableType = IRVariableType.Variable)
    {
        IRVariable var = new IRVariable(Counter.ToString(), type, variableType);
        Counter++;
        return var;
    }

    public string Build()
    {
        return string.Join('\n', _lines);
    }

    public void Comment(string comment, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 1)
            _lines.Add($"\t; {comment}");
    }

    public void Operation(string operation, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 2)
            _lines.Add($"\t; {file.Split('/').Last()}:{lineNumber}");
        _lines.Add($"\t{operation}");
    }

    public void Label(string label, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 2)
            _lines.Add($"\t; {file.Split('/').Last()}:{lineNumber}");
        _lines.Add(label);
    }

    public void Jump(string label, bool updateLastJumpPoint = true, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 2)
            _lines.Add($"\t; {file.Split('/').Last()}:{lineNumber}");
        _lines.Add($"\br label %{label}");
        if (updateLastJumpPoint)
            LastJumpPoint = label;
    }

    public void Branch(IRVariable condition, string labelIfTrue, string labelIfFalse, bool updateLastJumpPoint = true, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 2)
            _lines.Add($"\t; {file.Split('/').Last()}:{lineNumber}");
        _lines.Add($"\br i1 {condition.Reference}, label %{labelIfTrue}, label %{labelIfFalse}");
        if (updateLastJumpPoint)
            LastJumpPoint = labelIfFalse;
    }

    public IRVariable DereferenceIfPointer(IRVariable var, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (var.VariableType != IRVariableType.Pointer)
        {
            return var;
        }
        else
        {
            IRVariable tmp = NewVariable(var.Type, IRVariableType.Variable);
            Operation($"{tmp.Reference} = load {var.Type.IRType}, {var}");
            return tmp;
        }
    }
}