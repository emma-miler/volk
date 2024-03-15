using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Volk.Core.Objects;
using Volk.Std;

namespace Volk.Core;
public class VKProgram
{

    public VKScope RootScope { get; }

    public List<VKScope> Scopes = new();
    public List<VKFunction> Functions = new();

    Dictionary<string, VKCompileTimeString> _compileTimeStrings = new();
    public IEnumerable<VKCompileTimeString> CompileTimeStrings => _compileTimeStrings.Values;

    public VKProgram()
    {
        RootScope = new VKScope("__root", null!, VKType.INT);
        
        VKFunction mainFunc = new VKFunction(null!, "main", VKType.INT, true) {
            Scope = RootScope
        };
        Functions.Add(mainFunc);
        Scopes.Add(RootScope);

        SystemNative.AddBuiltinFunctions(this, RootScope);
        SystemNative.AddBuiltinTypes(RootScope);
        StdString.Add(this);
    }


    public void PrintExpressions()
    {
        Log.LogDetailLevel = Log.DetailLevel.None;
        foreach (VKFunction func in Functions)
        {
            // Dont print extern functions, since theyre empty anyway
            // and they end up just bloating the console output
            if (func is VKExternFunction)
                continue;
            Log.Info("");
            Log.Info("###############");
            Log.Info($"FUNC: {(func.IsStatic ? "static " : "")}{func.Name} ({string.Join(", ", func.Parameters.Select(x => $"{x.Type} {x.Name}"))})");
            Log.Info($"SCOPE: {func.Scope.ChainName}");
            Log.Info("###############");
            foreach (Expression expr in func.Scope.Expressions)
            {
                expr.Print(0);
            }
        }
        Log.LogDetailLevel = Log.DetailLevel.Detailed;
    }

    public VKCompileTimeString AddCompileTimeString(string value)
    {
        _compileTimeStrings.TryGetValue(value, out VKCompileTimeString? existing);
        if (existing != null) return existing;
        VKCompileTimeString cts = new(value, _compileTimeStrings.Count);
        _compileTimeStrings[value] = cts;
        return cts;
    }
}