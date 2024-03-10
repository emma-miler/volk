using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core.Objects;

namespace Volk.Core
{
    public class FunctionGroup
    {
        public IEnumerable<VKFunction> Functions => _functions;
        List<VKFunction> _functions;
        string _groupName;

        public FunctionGroup(string groupName, List<VKFunction> functions)
        {
            _groupName = groupName;
            _functions = functions;
        }

        public void AddFunction(VKFunction func)
        {
            if (_functions.Any(x => 
                x.Parameters.Count == func.Parameters.Count() &&
                x.Parameters.Zip(func.Parameters).All(p => p.First.Type == p.Second.Type)
            ))
                throw new TypeException($"Function group '{_groupName}' already contains a function with parameter types: ({string.Join(", ", func.Parameters.Select(x => $"{x.Type}"))})", func.Token!);
            _functions.Add(func);
        }
    }
}