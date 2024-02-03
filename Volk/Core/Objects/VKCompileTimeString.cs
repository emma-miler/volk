using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public record struct VKCompileTimeString
{

    public string Value;
    public int Index;

    public VKCompileTimeString(string value, int index)
    {
        Value = value;
        Index = index;
    }

    Dictionary<char, string> _llvmPatterns = new() {
        { '\\' , "\\5C" },
        { '\n', "\\0A" },
        { '\r', "\\0C" },
        { '\t', "\\09" },
        { '\"', "\\22" }
    };

    public string AsLLVMString()
    {
        string result = "";
        foreach (char c in Value)
        {
            if (_llvmPatterns.TryGetValue(c, out string? pattern))
                result += pattern;
            else
                result += c;
        }
        return result;
    }
}