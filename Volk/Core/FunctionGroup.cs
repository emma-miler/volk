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

        public IEnumerable<VKFunction> FindFunction(bool isStatic, IEnumerable<VKType> argumentTypes)
        {
            return Functions.Where(x => { 
                int indexOfVarargs = x.Parameters.FindIndex(x => x.Type == VKType.BUILTIN_C_VARARGS);
                IEnumerable<VKObject> parameters = x.Parameters;
                // If the function is non-static, we need to skip the first "this" parameter
                if (x.IsStatic != isStatic)
                    return false;
                // Special handling for functions with varargs
                if (indexOfVarargs > -1)
                {
                    parameters = parameters.Take(indexOfVarargs);
                    argumentTypes = argumentTypes.Take(indexOfVarargs);
                }
                bool paramCountsMatch = parameters.Count() == argumentTypes.Count();
                bool paramTypesMatch = parameters.Zip(argumentTypes).All(param => VKType.IsEqualOrDerived(param.First.Type, param.Second));
                return paramCountsMatch && paramTypesMatch;
            });
        }
    }
}