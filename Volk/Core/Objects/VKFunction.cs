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

        foreach (VKObject param in Parameters)
            Scope.AddObject(param);
    }

    public override string ToString()
    {
        return $"function {ReturnType} {Name} ({string.Join(',', Parameters)})";
    }

    public virtual void GenerateCode(CodeGenerator gen)
    {
        gen.Counter = 0;
        string parameters = string.Join(", ", Parameters.Select(x => $"{x.Type.IRType} noundef %param.{x.Name}"));
        // Add 2 newlines
        gen.Label("", silent: true);
        gen.Label("", silent: true);
        string header = $"define dso_local {(ReturnType == VKType.BUILTIN_VOID ? "" : "noundef")} {ReturnType.IRType} @{Name} ({parameters}) #0 {{";
        gen.Label(header);
        gen.Label("entry:");
        foreach (VKObject parameter in Parameters)
        {
            gen.Operation($"%{parameter.Name} = alloca {parameter.Type.IRType}, align 4");
            gen.Operation($"store {parameter.Type.IRType} %param.{parameter.Name}, ptr %{parameter.Name}, align 4");
        }
        gen.Counter = 0;
        Scope.GenerateCode(gen, true);
        gen.Label("}");
    }

    public virtual IRVariable CallInIR(CodeGenerator gen, params IRVariable[] arguments)
    {
        IRVariable retVal = gen.NewVariable(ReturnType);
        string ir;
        if (ReturnType == VKType.BUILTIN_VOID)
            ir = $"call {ReturnType.IRType} @{Name}(";
        else
            ir = $"{retVal.Reference} = call noundef {ReturnType.IRType} @{Name}(";
        ir += string.Join(", ", arguments);
        ir += ")";
        gen.Operation(ir);
        return retVal;
    }
}