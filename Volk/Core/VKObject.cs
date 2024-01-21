using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core;
public class VKObject
{
    public string Name { get; }

    public VKObject(string name)
    {
        Name = name;
    }
}