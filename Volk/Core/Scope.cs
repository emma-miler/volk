using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core;
public class Scope : VKObject
{
    public List<Expression> Expressions = new();

    public VKType ReturnType { get; }
    public Scope ParentScope { get; }

    public Scope(string name, Scope parentScope, VKType returnType) : base(name)
    {
        ParentScope = parentScope;
        ReturnType = returnType;
    }

}