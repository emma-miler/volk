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
        foreach (Scope scope in Scopes)
        {
            Log.Info($"SCOPE: {scope.ChainName}");
            foreach (Expression expr in scope.Expressions)
            {
                expr.Print(0);
            }
        }
        Log.LogDetailLevel = Log.DetailLevel.Detailed;
    }
}