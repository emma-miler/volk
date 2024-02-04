using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Volk.Core.Objects;

namespace Volk.Core;
public class VKProgram
{

    public List<Scope> Scopes = new();
    public List<VKFunction> Functions = new();

    public List<VKCompileTimeString> CompileTimeStrings = new();

    public VKProgram()
    {

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
}