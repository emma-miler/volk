using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public class VKNativeFunction : VKFunction
{

    Func<CodeGenerator, IRVariable[], IRVariable> _function;

    public VKNativeFunction(string name, VKType returnType, List<VKObject> parameters, Scope parentScope, Func<CodeGenerator, IRVariable[], IRVariable> function) : base(name, returnType, parameters, parentScope)
    {
        _function = function;
    }

    public override IRVariable CallInIR(CodeGenerator gen, IRVariable[] arguments)
    {
        return _function.Invoke(gen, arguments);
    }

}