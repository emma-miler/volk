using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Volk.Core.Objects;

namespace Volk.Core;
public class VKProgram
{

    public Scope RootScope { get; }

    public List<Scope> Scopes = new();
    public List<VKFunction> Functions = new();

    Dictionary<string, VKCompileTimeString> _compileTimeStrings = new();
    public IEnumerable<VKCompileTimeString> CompileTimeStrings => _compileTimeStrings.Values;

    public VKProgram()
    {
        RootScope = new Scope("__root", null!, VKType.BUILTIN_INT);
        RootScope.AddObject(VKType.BUILTIN_BOOL);
        RootScope.AddObject(VKType.BUILTIN_REAL);
        RootScope.AddObject(VKType.BUILTIN_INT);
        RootScope.AddObject(VKType.BUILTIN_VOID);
        RootScope.AddObject(VKType.BUILTIN_STRING);
        VKFunction mainFunc = new VKFunction("main", VKType.BUILTIN_INT, new(), null!) {
            Scope = RootScope
        };
        Functions.Add(mainFunc);
        Scopes.Add(RootScope);

        RootScope.AddObject(
            new VKFunction(
                "printf", 
                VKType.BUILTIN_INT, 
                new List<VKObject>() { 
                    new VKObject("format_string", VKType.BUILTIN_STRING),
                    new VKObject("args", VKType.BUILTIN_C_VARARGS),
                },
                RootScope
            )
        );

        BuiltinTypes.AddBuiltinTypes(RootScope);
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