using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core.Objects;

namespace Volk.Core;
public class SystemNative
{

    public static void AddBuiltinTypes(VKScope rootScope)
    {

        rootScope.AddType(VKType.BOOL);
        rootScope.AddType(VKType.REAL);
        rootScope.AddType(VKType.INT);
        rootScope.AddType(VKType.VOID);
        rootScope.AddType(VKType.STRING);
        rootScope.AddType(VKType.SYSTEM_POINTER);

        VKObject[] intParams = new[]
        {
            new VKObject("left", VKType.INT),
            new VKObject("right", VKType.INT),
        };
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Lt.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Lt, a, b, c); }, intParams));
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Le.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Le, a, b, c); }, intParams));
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Gt.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Gt, a, b, c); }, intParams));
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Ge.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Ge, a, b, c); }, intParams));
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Eq.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Eq, a, b, c); }, intParams));
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Ne.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Ne, a, b, c); }, intParams));

        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Add.ToString(), VKType.INT, (a,b,c) => { return ArithmeticOperation(OperatorType.Add, a, b, c); }, intParams));
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Sub.ToString(), VKType.INT, (a,b,c) => { return ArithmeticOperation(OperatorType.Sub, a, b, c); }, intParams));
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Mul.ToString(), VKType.INT, (a,b,c) => { return ArithmeticOperation(OperatorType.Mul, a, b, c); }, intParams));

        // Special case for int/int division resulting in a real
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Div.ToString(), VKType.REAL, (a,b,c) => {
            IRVariable left = CastIntToReal(VKType.REAL, b, new IRVariable[] { c[0] });
            IRVariable right = CastIntToReal(VKType.REAL, b, new IRVariable[] { c[1] });
            return ArithmeticOperation(OperatorType.Div, a, b, new IRVariable[] {left, right}); 
        }, intParams));

        VKObject[] intCastParams = new[]
        {
            new VKObject("left", VKType.INT),
        };
        VKType.BOOL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Cast.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Ne, a, b, new IRVariable[] { c.Single(), new IRVariable("0", VKType.INT, IRVariableType.Immediate)}); }, intCastParams));
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Cast.ToString(), VKType.REAL, (a,b,c) => { return CastIntToReal(a, b, c); }, intCastParams));



        VKObject[] realParams = new[]
        {
            new VKObject("left", VKType.REAL),
            new VKObject("right", VKType.REAL),
        };
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Lt.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Lt, a, b, c); }, realParams));
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Le.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Le, a, b, c); }, realParams));
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Gt.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Gt, a, b, c); }, realParams));
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Ge.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Ge, a, b, c); }, realParams));
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Eq.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Eq, a, b, c); }, realParams));
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Ne.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Ne, a, b, c); }, realParams));

        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Add.ToString(), VKType.REAL, (a,b,c) => { return ArithmeticOperation(OperatorType.Add, a, b, c); }, realParams));
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Sub.ToString(), VKType.REAL, (a,b,c) => { return ArithmeticOperation(OperatorType.Sub, a, b, c); }, realParams));
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Mul.ToString(), VKType.REAL, (a,b,c) => { return ArithmeticOperation(OperatorType.Mul, a, b, c); }, realParams));
        VKType.REAL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Div.ToString(), VKType.REAL, (a,b,c) => { return ArithmeticOperation(OperatorType.Div, a, b, c); }, realParams));
        VKObject[] realCastParams = new[]
        {
            new VKObject("left", VKType.REAL),
        };
        VKType.INT.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Cast.ToString(), VKType.INT, (a,b,c) => { return CastRealToInt(a, b, c); }, realCastParams));



        VKObject[] boolParams = new[]
        {
            new VKObject("left", VKType.BOOL),
            new VKObject("right", VKType.BOOL),
        };
        VKType.BOOL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Eq.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Eq, a, b, c); }, boolParams));
        VKType.BOOL.AddFunction(new VKNativeFunction(rootScope, "__" + OperatorType.Ne.ToString(), VKType.BOOL, (a,b,c) => { return Compare(OperatorType.Ne, a, b, c); }, boolParams));
    }

    public static void AddBuiltinFunctions(VKProgram program, VKScope scope)
    {
        VKExternFunction printf = new VKExternFunction(scope, "printf", VKType.INT, 
            new VKObject("format_string", VKType.STRING),
            new VKObject("args", VKType.BUILTIN_C_VARARGS)
        );
        scope.AddFunction(printf);
        program.Functions.Add(printf);
        

        VKExternFunction malloc = new VKExternFunction(scope, "malloc", VKType.SYSTEM_POINTER, new VKObject("size", VKType.INT));
        scope.AddFunction(malloc);
        program.Functions.Add(malloc);

        VKExternFunction free = new VKExternFunction(scope, "free", VKType.VOID, new VKObject("address", VKType.SYSTEM_POINTER));
        scope.AddFunction(free);
        program.Functions.Add(free);

        VKExternFunction memset = new VKExternFunction(scope, "llvm.memset.p0.i64", VKType.VOID, 
                new VKObject("dest", VKType.SYSTEM_POINTER),
                new VKObject("value", VKType.BUILTIN_C_BYTE),
                new VKObject("length", VKType.INT),
                new VKObject("isVolatile", VKType.BOOL)
        );
        scope.AddFunction(memset);
        program.Functions.Add(memset);
    }

    static IRVariable Compare(OperatorType opType, VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        string cmpOp;
        string op = OperatorToken.GetIROperator(opType);
        if (opType == OperatorType.Gt || opType == OperatorType.Ge || opType == OperatorType.Lt || opType == OperatorType.Le)
            op = "s" + op;
        VKType operationType = args[0].Type;
        if (operationType == VKType.INT) cmpOp = "icmp";
        else if (operationType == VKType.REAL) cmpOp = "fcmp";
        else if (operationType == VKType.BOOL) cmpOp = "icmp";
        else throw new InvalidEnumArgumentException($"Invalid VKType '{operationType}'");
        IRVariable retVal = gen.NewVariable(returnType);
        gen.Operation($"{retVal.Reference} = {cmpOp} {op} {operationType.IRType} {args[0].Reference}, {args[1].Reference}");
        return retVal;
    }

    static IRVariable ArithmeticOperation(OperatorType opType, VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        string op = OperatorToken.GetIROperator(opType);
        VKType operationType = args[0].Type;

        if (operationType == VKType.REAL) op = "f" + op;
        else if (opType == OperatorType.Div || opType == OperatorType.Mod) op = "s" + op;
        
        IRVariable retVal = gen.NewVariable(returnType);
        gen.Operation($"{retVal.Reference} = {op} {operationType.IRType} {args[1].Reference}, {args[0].Reference}");
        return retVal;
    }

    #region Value Casting

    static IRVariable CastIntToBool(VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        IRVariable var = gen.DecayToVariable(args[0]);
        IRVariable ret = gen.NewVariable(returnType);
        gen.Operation($"{ret.Reference} = trunc {var} to {returnType.IRType}");
        return ret;
    }

    static IRVariable CastIntToReal(VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        IRVariable var = gen.DecayToVariable(args[0]);
        IRVariable ret = gen.NewVariable(returnType);
        gen.Operation($"{ret.Reference} = sitofp {var} to {returnType.IRType}");
        return ret;
    }

    static IRVariable CastRealToInt(VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        IRVariable var = gen.DecayToVariable(args[0]);
        IRVariable ret = gen.NewVariable(returnType);
        gen.Operation($"{ret.Reference} = fptosi {var} to {returnType.IRType}");
        return ret;
    }

    #endregion

}