using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public record VKCompileTimeString
{

    public string Value { get; }
    public int Index { get; }

    Dictionary<string, string> _desanitizePatterns = new() {
        { "\\\\" , "\\" },
        { "\\n", "\n" },
        { "\\r", "\r" },
        { "\\t", "\t" },
        { "\\\"", "\"" }
    };


    public VKCompileTimeString(string value, int index)
    {
        Value = value;
        Index = index;

        foreach ((string pattern, string replace) in _desanitizePatterns)
        {
            Value = Value.Replace(pattern, replace);
        }
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