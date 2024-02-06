using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core.Objects;

namespace Volk.Core;
public class BuiltinTypes
{

    public static void AddBuiltinTypes(Scope rootScope)
    {
        List<VKObject> intParams = new()
        {
            new VKObject("left", VKType.BUILTIN_INT),
            new VKObject("right", VKType.BUILTIN_INT),
        };
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Lt.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b) => { return Compare(OperatorType.Lt, a, VKType.BUILTIN_INT, b); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Le.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b) => { return Compare(OperatorType.Le, a, VKType.BUILTIN_INT, b); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Gt.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b) => { return Compare(OperatorType.Gt, a, VKType.BUILTIN_INT, b); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Ge.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b) => { return Compare(OperatorType.Ge, a, VKType.BUILTIN_INT, b); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Eq.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b) => { return Compare(OperatorType.Eq, a, VKType.BUILTIN_INT, b); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Ne.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b) => { return Compare(OperatorType.Ne, a, VKType.BUILTIN_INT, b); }));

        List<VKObject> realParams = new()
        {
            new VKObject("left", VKType.BUILTIN_REAL),
            new VKObject("right", VKType.BUILTIN_REAL),
        };
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Lt.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b) => { return Compare(OperatorType.Lt, a, VKType.BUILTIN_REAL, b); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Le.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b) => { return Compare(OperatorType.Le, a, VKType.BUILTIN_REAL, b); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Gt.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b) => { return Compare(OperatorType.Gt, a, VKType.BUILTIN_REAL, b); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Ge.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b) => { return Compare(OperatorType.Ge, a, VKType.BUILTIN_REAL, b); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Eq.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b) => { return Compare(OperatorType.Eq, a, VKType.BUILTIN_REAL, b); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Ne.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b) => { return Compare(OperatorType.Ne, a, VKType.BUILTIN_REAL, b); }));

        List<VKObject> boolParams = new()
        {
            new VKObject("left", VKType.BUILTIN_BOOL),
            new VKObject("right", VKType.BUILTIN_BOOL),
        };
        VKType.BUILTIN_BOOL.Methods.Add(new VKNativeFunction("__" + OperatorType.Eq.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b) => { return Compare(OperatorType.Eq, a, VKType.BUILTIN_BOOL, b); }));
        VKType.BUILTIN_BOOL.Methods.Add(new VKNativeFunction("__" + OperatorType.Ne.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b) => { return Compare(OperatorType.Ne, a, VKType.BUILTIN_BOOL, b); }));
    }

    public static IRVariable Compare(OperatorType opType, CodeGenerator gen, VKType returnType, IRVariable[] args)
    {
        string cmpOp;
        string op = OperatorToken.GetIROperator(opType);
        if (opType == OperatorType.Gt || opType == OperatorType.Ge || opType == OperatorType.Lt || opType == OperatorType.Le)
            op = "s" + op;
        if (returnType == VKType.BUILTIN_INT) cmpOp = "icmp";
        else if (returnType == VKType.BUILTIN_REAL) cmpOp = "fcmp";
        else if (returnType == VKType.BUILTIN_BOOL) cmpOp = "icmp";
        else throw new InvalidEnumArgumentException($"Invalid VKType '{returnType}'");
        IRVariable retVal = gen.NewVariable(VKType.BUILTIN_BOOL);
        gen.Operation($"{retVal.Reference} = {cmpOp} {op} {returnType.IRType} {args[1].Reference}, {args[0].Reference}");
        return retVal;
    }

}