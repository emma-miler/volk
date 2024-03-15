using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core.Expressions;

namespace Volk.Core.Objects;
public class VKType : VKScope
{
    public static VKType TYPE = new VKType("type", true, isBuiltin: true);
    public static VKType BOOL = new VKType("bool", false, irType: "i1", isBuiltin: true);
    public static VKType REAL = new VKType("real", false, irType: "double", isBuiltin: true);
    public static VKType INT = new VKType("int", false, irType: "i64", isBuiltin: true);
    public static VKType VOID = new VKType("void", false, irType: "void", isBuiltin: true);
    public static VKType STRING = null!;
    public static VKType SYSTEM_ERROR = new VKType("__builtin_error", false, isBuiltin: true);
    public static VKType BUILTIN_FUNCTION = new VKType("function", true, isBuiltin: true);
    public static VKType BUILTIN_C_BYTE = new VKType("__byte", false, irType: "i8", isBuiltin: true);
    public static VKType SYSTEM_GENERIC_POINTER = new VKType("_generic_ptr", false, irType: "i8*", isBuiltin: true);
    public static VKType SYSTEM_C_STRING = new VKType("__cstring", false, irType: "i8*", isBuiltin: true);
    public static VKType BUILTIN_C_VARARGS = new VKType("__varargs", true, irType: "i8*", isBuiltin: true);
    
    public bool IsReferenceType { get; }
    public string? IRType { get; }
    public bool IsBasicType { get; }
    public bool IsBuiltin { get; }

    public List<VKField> Fields = new();

    public VKType(string name, bool isReferenceType, VKScope? parentScope = null, string? irType = null, bool isBuiltin = false) : base(name, parentScope, VKType.VOID)
    {
        IsReferenceType = isReferenceType;
        IRType = irType ?? (IsBasicType ? $"__INVALID_IR_TYPE({name})" : $"%class.{name}*");
        Type = VKType.TYPE;
        IsBuiltin = isBuiltin;
        IsBasicType = irType != null;
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
        return FindFunction($"__{OperatorType.Cast}", true, new List<VKType> { target }).FirstOrDefault();
    }

    public override VKObject AddObject(VKObject obj)
    {
        int offset;
        if (Fields.Any())
            offset = Fields.Last().Offset + 1;
        else
            offset = 0;
        VKField field = new VKField(obj.Token!, obj.Name, obj.Type, offset);
        Fields.Add(field);
        return field;
    }

    public override VKObject? FindVariable(string name)
    {
        VKObject? obj = Fields.FirstOrDefault(x => x.Name == name);
        if (obj != null)
            return obj;
        return base.FindVariable(name);
    }

    public override IRVariable GenerateCode(CodeGenerator gen, bool forceReturnValue)
    {
        if (!IsBuiltin)
        {
            gen.Label($"%class.{Name} = type {{ {string.Join(", ", Fields.Select(x => x.Type.IRType))} }}");
        }
        return new IRVariable("__null", VKType.VOID, IRVariableType.Immediate);
    }
}