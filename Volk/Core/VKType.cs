using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core;
public class VKType : VKObject
{
    public static VKType BUILTIN_BOOL = new VKType("bool", false, "i1");
    public static VKType BUILTIN_REAL = new VKType("bool", false, "double");
    public static VKType BUILTIN_INT = new VKType("bool", false, "i64");
    public static VKType BUILTIN_VOID = new VKType("void", false, "void");


    bool IsReferenceType { get; }
    string? LLVMType { get; }

    public VKType(string name, bool isReferenceType, string? llvmType = null) : base(name)
    {
        IsReferenceType = isReferenceType;
        LLVMType = llvmType;
    } 
}