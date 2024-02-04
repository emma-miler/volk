using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core.Objects;

namespace Volk.Core;
public class Scope : VKObject
{
    public List<Expression> Expressions = new();
    Dictionary<string, VKObject> _objects = new();

    public VKType ReturnType { get; }
    Scope? _parentScope { get; }

    public string ChainName => _parentScope?.ChainName ?? "" + Name;

    public Scope(string name, Scope parentScope, VKType returnType) : base(name, VKType.BUILTIN_SYSTEM_VOID)
    {
        _parentScope = parentScope;
        ReturnType = returnType;
    }
    
    public void AddObject(VKObject obj)
    {
        if (_objects.ContainsKey(obj.Name))
            throw new DuplicateNameException($"Object with name '{obj.Name}' already exists in scope");
        _objects[obj.Name] = obj;
    }

    public void CloseScope()
    {
        if (ReturnType != VKType.BUILTIN_VOID && Expressions.Last().ExpressionType != ExpressionType.Return)
            throw new Exception($"Cannot exit non-void scope '{Name}' with return type '{ReturnType}' without a return statement");
    }

    public VKObject? FindVariable(string name)
    {
        _objects.TryGetValue(name, out VKObject? obj);
        if (obj != null && obj is VKObject varObj)
            return varObj;
        else if (_parentScope != null)
            return _parentScope.FindVariable(name);
        else 
            return null;
    }

    public VKType? FindType(string name)
    {
        _objects.TryGetValue(name, out VKObject? obj);
        if (obj != null && obj is VKType typeObj)
            return typeObj;
        else if (_parentScope != null)
            return _parentScope.FindType(name);
        else
            return null;
    }


}