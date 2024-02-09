using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public class VKType : VKObject
{
    public static VKType BUILTIN_TYPE = new VKType("type", true);
    public static VKType BUILTIN_BOOL = new VKType("bool", false, "i1");
    public static VKType BUILTIN_REAL = new VKType("real", false, "double");
    public static VKType BUILTIN_INT = new VKType("int", false, "i64");
    public static VKType BUILTIN_VOID = new VKType("void", false, "void");
    public static VKType BUILTIN_STRING = new VKType("string", false, "ptr");
    public static VKType BUILTIN_SYSTEM_VOID = new VKType("__system_void", false);
    public static VKType BUILTIN_SYSTEM_POINTER = new VKType("__system_ptr", false, "ptr");
    public static VKType BUILTIN_ERROR = new VKType("__builtin_error", false);
    public static VKType BUILTIN_FUNCTION = new VKType("function", true);
    public static VKType BUILTIN_C_VARARGS = new VKType("__varargs", true);

    public List<VKFunction> Methods = new();


    bool IsReferenceType { get; }
    public string? IRType { get; }

    public VKType(string name, bool isReferenceType, string? irType = null) : base(name, BUILTIN_TYPE)
    {
        IsReferenceType = isReferenceType;
        IRType = irType ?? $"__INVALID_IR_TYPE({name})";
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
        return Methods.FirstOrDefault(
            x => x.Name == $"__{OperatorType.Cast}" && 
            x.Parameters.Count == 1 && 
            x.ReturnType == target
        );
    }
}