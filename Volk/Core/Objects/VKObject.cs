using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public class VKObject
{
    public VKType Type { get; protected set; }
    public string Name { get; init; }
    public Token? Token { get; init; }

    public VKObject(string name, VKType type, Token? token = null)
    {
        Name = name;
        Type = type;
        Token = token;
    }

    public void UpdateType(VKType type)
    {
        Type = type;
    }

    public override string ToString()
    {
        return $"'{Name}': {Type}";
    }
}