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
}