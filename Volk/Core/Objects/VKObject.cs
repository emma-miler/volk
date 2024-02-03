using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core.Objects;
public class VKObject
{
    public VKType Type { get; private set; }
    public string Name { get; }

    public VKObject(string name, VKType type)
    {
        Name = name;
        Type = type;
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