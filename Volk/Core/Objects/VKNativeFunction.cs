using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public class VKInlineFunction : VKFunction
{

    Func<VKType, CodeGenerator, IRVariable[], IRVariable> _function;

    public VKInlineFunction(VKScope parentScope, string name, VKType returnType, bool isStatic, Func<VKType, CodeGenerator, IRVariable[], IRVariable> function, params VKObject[] parameters) : base(parentScope, name, returnType, isStatic, parameters.ToArray())
    {
        _function = function;
    }

    public override IRVariable CallInIR(CodeGenerator gen, IRVariable[] arguments)
    {
        return _function.Invoke(ReturnType, gen, arguments);
    }

}