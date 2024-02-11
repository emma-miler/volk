using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using Volk.Core.Objects;

namespace Volk.Core;
public class CodeGenerator
{

    public List<string> Lines = new();
    public int Counter { get; set; } = 0;
    public string LastJumpPoint { get; set; } = string.Empty;

    public void AddStringTable(IEnumerable<VKCompileTimeString> stringObjects)
    {
         foreach (VKCompileTimeString str in stringObjects)
        {
            Lines.Add($"@.str.{str.Index} = private unnamed_addr constant [{str.Value.Length + 1} x i8] c\"{str.AsLLVMString() + "\\00"}\", align 1");
        }
    }

    public IRVariable NewVariable(VKType type, IRVariableType variableType = IRVariableType.Variable, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 3)
            Lines.Add($"\t\t; COUNTER {Counter} -> {Counter + 1} {file.Split('/').Last()}:{lineNumber}");
        IRVariable var = new IRVariable(Counter.ToString(), type, variableType);
        Counter++;
        return var;
    }

    public string Build()
    {
        return string.Join('\n', Lines);
    }

    public void Comment(string comment, bool silent = false, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 1 && !silent)
            Lines.Add($"\t; {comment}");
    }

    public void Operation(string operation, bool silent = false, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 2 && !silent)
            Lines.Add($"\t; {file.Split('/').Last()}:{lineNumber}");
        Lines.Add($"\t{operation}");
    }

    public void Label(string label, bool silent = false, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 2 && !silent)
            Lines.Add($"\t; {file.Split('/').Last()}:{lineNumber}");
        Lines.Add(label);
    }

    public void Jump(string label, bool silent = false, bool updateLastJumpPoint = true, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 2 && !silent)
            Lines.Add($"\t; {file.Split('/').Last()}:{lineNumber}");
        Lines.Add($"\tbr label %{label}");
        if (updateLastJumpPoint)
            LastJumpPoint = label;
    }

    public void Branch(IRVariable condition, string labelIfTrue, string labelIfFalse, bool updateLastJumpPoint = true, bool silent = false, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
    {
        if (RuntimeConfig.IRVerbosity >= 2 && !silent)
            Lines.Add($"\t; {file.Split('/').Last()}:{lineNumber}");
        Lines.Add($"\tbr i1 {condition.Reference}, label %{labelIfTrue}, label %{labelIfFalse}");
        if (updateLastJumpPoint)
            LastJumpPoint = labelIfFalse;
    }

    public IRVariable DecayToVariable(IRVariable var, [CallerFilePath] string file = "", [CallerLineNumber] int lineNumber = 0)
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