using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Volk.Core.Objects;

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
        RootScope.AddType(VKType.BOOL);
        RootScope.AddType(VKType.REAL);
        RootScope.AddType(VKType.INT);
        RootScope.AddType(VKType.VOID);
        RootScope.AddType(VKType.STRING);
        VKFunction mainFunc = new VKFunction(null!, "main", VKType.INT) {
            Scope = RootScope
        };
        Functions.Add(mainFunc);
        Scopes.Add(RootScope);

        SystemNative.AddBuiltinFunctions(this, RootScope);
        SystemNative.AddBuiltinTypes(RootScope);
    }


    public void PrintExpressions()
    {
        Log.LogDetailLevel = Log.DetailLevel.None;
        foreach (VKFunction func in Functions)
        {
            Log.Info($"FUNC: {func.Name}");
            Log.Info($"SCOPE: {func.Scope.ChainName}");
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