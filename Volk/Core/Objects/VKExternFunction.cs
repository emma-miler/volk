using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http.Headers;
using System.Security.Cryptography.X509Certificates;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public class VKExternFunction : VKFunction
{
    public VKExternFunction(VKScope parentScope, string name, VKType returnType, params VKObject[] parameters) : base(parentScope, name, returnType, parameters)
    {
    }

    public override void GenerateCode(CodeGenerator gen)
    {
        string parameters = string.Join(", ", Parameters.Select(x => $"{x.Type.IRType} noundef %param.{x.Name}"));
        gen.Label($"declare {ReturnType.IRType} @{MangledName} ({parameters}) #0");
    }
}