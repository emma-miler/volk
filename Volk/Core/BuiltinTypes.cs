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
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Lt.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b,c) => { return Compare(OperatorType.Lt, a, b, c); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Le.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b,c) => { return Compare(OperatorType.Le, a, b, c); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Gt.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b,c) => { return Compare(OperatorType.Gt, a, b, c); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Ge.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b,c) => { return Compare(OperatorType.Ge, a, b, c); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Eq.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b,c) => { return Compare(OperatorType.Eq, a, b, c); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Ne.ToString(), VKType.BUILTIN_BOOL, intParams, rootScope, (a,b,c) => { return Compare(OperatorType.Ne, a, b, c); }));

        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Add.ToString(), VKType.BUILTIN_INT, intParams, rootScope, (a,b,c) => { return ArithmeticOperation(OperatorType.Add, a, b, c); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Sub.ToString(), VKType.BUILTIN_INT, intParams, rootScope, (a,b,c) => { return ArithmeticOperation(OperatorType.Sub, a, b, c); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Mul.ToString(), VKType.BUILTIN_INT, intParams, rootScope, (a,b,c) => { return ArithmeticOperation(OperatorType.Mul, a, b, c); }));

        // Special case for int/int division resulting in a real
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Div.ToString(), VKType.BUILTIN_REAL, intParams, rootScope, (a,b,c) => {
            IRVariable left = CastIntToReal(VKType.BUILTIN_REAL, b, new IRVariable[] { c[0] });
            IRVariable right = CastIntToReal(VKType.BUILTIN_REAL, b, new IRVariable[] { c[1] });
            return ArithmeticOperation(OperatorType.Div, a, b, new IRVariable[] {left, right}); 
        }));

        List<VKObject> intCastParams = new()
        {
            new VKObject("left", VKType.BUILTIN_INT),
        };
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Cast.ToString(), VKType.BUILTIN_BOOL, intCastParams, rootScope, (a,b,c) => { return CastIntToBool(a, b, c); }));
        VKType.BUILTIN_INT.Methods.Add(new VKNativeFunction("__" + OperatorType.Cast.ToString(), VKType.BUILTIN_REAL, intCastParams, rootScope, (a,b,c) => { return CastIntToReal(a, b, c); }));



        List<VKObject> realParams = new()
        {
            new VKObject("left", VKType.BUILTIN_REAL),
            new VKObject("right", VKType.BUILTIN_REAL),
        };
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Lt.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b,c) => { return Compare(OperatorType.Lt, a, b, c); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Le.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b,c) => { return Compare(OperatorType.Le, a, b, c); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Gt.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b,c) => { return Compare(OperatorType.Gt, a, b, c); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Ge.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b,c) => { return Compare(OperatorType.Ge, a, b, c); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Eq.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b,c) => { return Compare(OperatorType.Eq, a, b, c); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Ne.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b,c) => { return Compare(OperatorType.Ne, a, b, c); }));

        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Add.ToString(), VKType.BUILTIN_REAL, realParams, rootScope, (a,b,c) => { return ArithmeticOperation(OperatorType.Add, a, b, c); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Sub.ToString(), VKType.BUILTIN_REAL, realParams, rootScope, (a,b,c) => { return ArithmeticOperation(OperatorType.Sub, a, b, c); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Mul.ToString(), VKType.BUILTIN_REAL, realParams, rootScope, (a,b,c) => { return ArithmeticOperation(OperatorType.Mul, a, b, c); }));
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Div.ToString(), VKType.BUILTIN_REAL, realParams, rootScope, (a,b,c) => { return ArithmeticOperation(OperatorType.Div, a, b, c); }));
        List<VKObject> realCastParams = new()
        {
            new VKObject("left", VKType.BUILTIN_REAL),
        };
        VKType.BUILTIN_REAL.Methods.Add(new VKNativeFunction("__" + OperatorType.Cast.ToString(), VKType.BUILTIN_INT, realCastParams, rootScope, (a,b,c) => { return CastRealToInt(a, b, c); }));



        List<VKObject> boolParams = new()
        {
            new VKObject("left", VKType.BUILTIN_BOOL),
            new VKObject("right", VKType.BUILTIN_BOOL),
        };
        VKType.BUILTIN_BOOL.Methods.Add(new VKNativeFunction("__" + OperatorType.Eq.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b,c) => { return Compare(OperatorType.Eq, a, b, c); }));
        VKType.BUILTIN_BOOL.Methods.Add(new VKNativeFunction("__" + OperatorType.Ne.ToString(), VKType.BUILTIN_BOOL, realParams, rootScope, (a,b,c) => { return Compare(OperatorType.Ne, a, b, c); }));
    }

    static IRVariable Compare(OperatorType opType, VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        string cmpOp;
        string op = OperatorToken.GetIROperator(opType);
        if (opType == OperatorType.Gt || opType == OperatorType.Ge || opType == OperatorType.Lt || opType == OperatorType.Le)
            op = "s" + op;
        VKType operationType = args[0].Type;
        if (operationType == VKType.BUILTIN_INT) cmpOp = "icmp";
        else if (operationType == VKType.BUILTIN_REAL) cmpOp = "fcmp";
        else if (operationType == VKType.BUILTIN_BOOL) cmpOp = "icmp";
        else throw new InvalidEnumArgumentException($"Invalid VKType '{operationType}'");
        IRVariable retVal = gen.NewVariable(returnType);
        gen.Operation($"{retVal.Reference} = {cmpOp} {op} {operationType.IRType} {args[1].Reference}, {args[0].Reference}");
        return retVal;
    }

    static IRVariable ArithmeticOperation(OperatorType opType, VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        string op = OperatorToken.GetIROperator(opType);
        VKType operationType = args[0].Type;

        if (operationType == VKType.BUILTIN_REAL) op = "f" + op;
        else if (opType == OperatorType.Div || opType == OperatorType.Mod) op = "s" + op;
        
        IRVariable retVal = gen.NewVariable(returnType);
        gen.Operation($"{retVal.Reference} = {op} {operationType.IRType} {args[1].Reference}, {args[0].Reference}");
        return retVal;
    }

    #region Value Casting

    static IRVariable CastIntToBool(VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        IRVariable var = gen.DereferenceIfPointer(args[0]);
        IRVariable ret = gen.NewVariable(returnType);
        gen.Operation($"{ret.Reference} = trunc {var} to {returnType.IRType}");
        return ret;
    }

    static IRVariable CastIntToReal(VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        IRVariable var = gen.DereferenceIfPointer(args[0]);
        IRVariable ret = gen.NewVariable(returnType);
        gen.Operation($"{ret.Reference} = sitofp {var} to {returnType.IRType}");
        return ret;
    }

    static IRVariable CastRealToInt(VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        IRVariable var = gen.DereferenceIfPointer(args[0]);
        IRVariable ret = gen.NewVariable(returnType);
        gen.Operation($"{ret.Reference} = fptosi {var} to {returnType.IRType}");
        return ret;
    }

    #endregion

}