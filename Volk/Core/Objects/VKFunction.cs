using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core.Objects;


public class VKFunction : VKObject
{
    public VKType ReturnType => Scope.ReturnType;

    public List<VKObject> Parameters { get; }

    public Scope Scope;

    public VKFunction(string name, VKType returnType, List<VKObject> parameters, Scope parentScope) : base(name, VKType.BUILTIN_FUNCTION)
    {
        Parameters = parameters;
        Scope = new Scope(name, parentScope, returnType);
    }

    public override string ToString()
    {
        return $"function {Name} ({string.Join(',', Parameters)})";
    }

    public void GenerateCode(CodeGenerator gen)
    {
        string parameters = string.Join(", ", Parameters.Select(x => $"{x.Type.IRType} noundef %param.{x.Name}"));
        string header = $"define dso_local {(ReturnType == VKType.BUILTIN_VOID ? "" : "noundef")} {ReturnType.IRType} @{Name} ({parameters}) #0 {{";
        gen.Label(header);
        gen.Label("entry:");
        foreach (VKObject parameter in Parameters)
        {
            gen.Operation($"%{parameter.Name} = alloca {parameter.Type.IRType}, align 4");
            gen.Operation($"store {parameter.Type.IRType} %param.{parameter.Name}, ptr %{parameter.Name}, align 4");
        }

        foreach (Expression expr in Scope.Expressions)
        {
            expr.GenerateCode(gen);
        }
        gen.Operation($"ret void");
        gen.Label("}");
    }
}