using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public class VKNativeFunction : VKFunction
{

    Func<VKType, CodeGenerator, IRVariable[], IRVariable> _function;

    public VKNativeFunction(VKScope parentScope, string name, VKType returnType, Func<VKType, CodeGenerator, IRVariable[], IRVariable> function, params VKObject[] parameters) : base(parentScope, name, returnType, true, parameters.ToArray())
    {
        _function = function;
    }

    public override IRVariable CallInIR(CodeGenerator gen, IRVariable[] arguments)
    {
        return _function.Invoke(ReturnType, gen, arguments);
    }

}