using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public class VKType : VKScope
{
    public static VKType TYPE = new VKType("type", true, isBuiltin: true);
    public static VKType BOOL = new VKType("bool", false, irType: "i1", isBuiltin: true);
    public static VKType REAL = new VKType("real", false, irType: "double", isBuiltin: true);
    public static VKType INT = new VKType("int", false, irType: "i64", isBuiltin: true);
    public static VKType VOID = new VKType("void", false, irType: "void", isBuiltin: true);
    public static VKType STRING = new VKType("string", false, irType: "ptr", isBuiltin: true);
    public static VKType SYSTEM_POINTER = new VKType("__system_ptr", false, irType: "ptr", isBuiltin: true);
    public static VKType SYSTEM_ERROR = new VKType("__builtin_error", false, isBuiltin: true);
    public static VKType BUILTIN_FUNCTION = new VKType("function", true, isBuiltin: true);
    public static VKType BUILTIN_C_VARARGS = new VKType("__varargs", true, isBuiltin: true);
    public static VKType BUILTIN_C_STRING = new VKType("__cstring", true, irType: "ptr", isBuiltin: true);
    public static VKType BUILTIN_C_BYTE = new VKType("__byte", false, irType: "i8", isBuiltin: true);

    public bool IsReferenceType { get; }
    public string? IRType { get; }
    public bool IsBasicType { get; }
    public bool IsBuiltin { get; }

    public VKType(string name, bool isReferenceType, VKScope? parentScope = null, string? irType = null, bool isBuiltin = false) : base(name, parentScope, VKType.VOID)
    {
        IsReferenceType = isReferenceType;
        IRType = irType ?? (IsBasicType ? $"__INVALID_IR_TYPE({name})" : "ptr");
        Type = VKType.TYPE;
        IsBuiltin = isBuiltin;
        IsBasicType = irType != null;

        if (!isBuiltin)
        {
            AddFunction(new VKNativeFunction(parentScope!, "__constructor", this, MallocConstructor));
        }
    }

    public override string ToString()
    {
        return $"{Name}{ (IsReferenceType ? "&" : "") }";
    }

    public static bool IsEqualOrDerived(VKType left, VKType right)
    {
        return left == right;
    }

    public VKFunction? FindCastFunction(VKType target)
    {
        return Functions.FirstOrDefault(
            x => x.Name == $"__{OperatorType.Cast}" && 
            x.Parameters.Count == 1 && 
            x.ReturnType == target
        );
    }

    public override IRVariable GenerateCode(CodeGenerator gen, bool forceReturnValue)
    {
        if (!IsBuiltin)
        {
            gen.Label($"%class.{Name} = type {{ {string.Join(", ", Fields.Select(x => x.Type.IsBasicType ? x.Type.IRType : "ptr"))} }}");
        }
        return new IRVariable("__null", VKType.VOID, IRVariableType.Immediate);
    }

    static IRVariable MallocConstructor(VKType returnType, CodeGenerator gen, IRVariable[] args)
    {
        IRVariable ret = gen.NewVariable(VKType.SYSTEM_POINTER);
        gen.Operation($"{ret.Reference} = call noalias ptr @malloc(i64 noundef {returnType.Fields.Count() * 8})");
        return ret;
    }
}