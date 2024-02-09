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

    public string MangledName => Scope.ChainName == "" ? Name : Scope.ChainName;

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
        string header = $"define dso_local {(ReturnType == VKType.BUILTIN_VOID ? "" : "noundef ")}{ReturnType.IRType} @{MangledName} ({parameters}) #0 {{";
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
        IRVariable retVal;
        string ir;
        if (ReturnType == VKType.BUILTIN_VOID)
        {
            retVal = new IRVariable("__void", VKType.BUILTIN_ERROR, IRVariableType.Variable);
            ir = $"call {ReturnType.IRType} @{MangledName}(";
        }
        else
        {
            retVal = gen.NewVariable(ReturnType);
            ir = $"{retVal.Reference} = call noundef {ReturnType.IRType} @{MangledName}(";
        }
        ir += string.Join(", ", arguments);
        ir += ")";
        gen.Operation(ir);
        return retVal;
    }
}