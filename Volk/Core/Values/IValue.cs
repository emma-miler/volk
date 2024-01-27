using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core.Objects;

namespace Volk.Core.Values;
public interface IValue
{
    public VKType? ValueType { get; }
}