using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public class VKField : VKObject
{
    public int Offset { get; }
    public string TypeName { get; } = string.Empty;

    public VKField(Token token, string name, VKType type, int offset) : base(name, type, token)
    {
        Offset = offset;
    }

    public VKField(Token token, string name, string typeName, int offset) : base(name, VKType.SYSTEM_ERROR.Type, token)
    {
        Offset = offset;
        TypeName = typeName;
    }
}