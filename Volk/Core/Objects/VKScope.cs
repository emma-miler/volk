using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Linq.Expressions;
using System.Threading.Tasks;
using Volk.Core.Expressions;
using Volk.Core.Objects;

namespace Volk.Core;
public class VKScope : VKObject
{
    public List<Expression> Expressions = new();
    List<VKType> _types = new();
    List<VKObject> _variables = new();
    Dictionary<string, FunctionGroup> _functionGroups = new();

    public IEnumerable<VKType> Types => _types;
    public IEnumerable<VKObject> Variables => _variables;
    public IEnumerable<FunctionGroup> FunctionGroups => _functionGroups.Values;


    public VKType ReturnType { get; }
    VKScope? _parentScope { get; }

    public string ChainName
    {
        get
        {
            if (_parentScope == null) return "";
            else if (_parentScope.ChainName == "") return Name;
            else return _parentScope.ChainName + "$" + Name;
        }
    }

    public VKScope(string name, VKScope? parentScope, VKType returnType) : base(name, VKType.VOID)
    {
        _parentScope = parentScope;
        ReturnType = returnType;
    }

     public virtual VKObject AddType(VKType obj)
    {
        if (_types.Any(x => x.Name == obj.Name))
            throw new DuplicateNameException($"Object with name '{obj.Name}' already exists in scope");
        _types.Add(obj);
        return obj;
    }

    public virtual VKObject AddObject(VKObject obj)
    {
        if (_variables.Any(x => x.Name == obj.Name))
            throw new DuplicateNameException($"Object with name '{obj.Name}' already exists in scope");
        _variables.Add(obj);
        return obj;
    }

    public virtual VKObject AddFunction(VKFunction obj)
    {
        if (!_functionGroups.ContainsKey(obj.Name))
            _functionGroups[obj.Name] = new FunctionGroup(obj.Name,new List<VKFunction> {obj});
        else
        {
            FunctionGroup group = _functionGroups[obj.Name];
            group.AddFunction(obj);
        }
        return obj;
    }

    public void CloseScope()
    {
        if (ReturnType != VKType.VOID && Expressions.Last().ExpressionType != ExpressionType.Return)
            throw new Exception($"Cannot exit non-void scope '{Name}' with return type '{ReturnType}' without a return statement");
    }

    public virtual VKObject? FindVariable(string name)
    {
        VKObject? obj = _variables.Find(x => x.Name == name);
        if (obj != null && obj is VKObject varObj)
            return varObj;
        else if (_parentScope != null)
            return _parentScope.FindVariable(name);
        else
            return null;
    }

    public virtual FunctionGroup? FindFunctionGroup(string name)
    {
        if (_functionGroups.TryGetValue(name, out FunctionGroup? group))
            return group;
        else 
            return _parentScope?.FindFunctionGroup(name);
    }

    public virtual IEnumerable<VKFunction> FindFunction(string name, bool isStatic, IEnumerable<VKType> argumentTypes)
    {
        if (!_functionGroups.TryGetValue(name, out FunctionGroup? group))
            return _parentScope!.FindFunction(name, isStatic, argumentTypes);
        return group.FindFunction(isStatic, argumentTypes);
    }
    

    public VKType? FindType(string name)
    {
        VKType? obj = _types.Find(x => x.Name == name);
        if (obj != null)
            return obj;
        else if (_parentScope != null)
            return _parentScope.FindType(name);
        else
            return null;
    }

    public virtual IRVariable GenerateCode(CodeGenerator gen, bool forceReturnValue)
    {
        IRVariable last = new IRVariable("__ret", ReturnType, IRVariableType.Immediate);

        foreach (Expression expr in Expressions)
        {
            last = expr.GenerateCode(gen);
        }

        // Add default return value for scope
        if (forceReturnValue && (!Expressions.Any() || Expressions.Last().ExpressionType != ExpressionType.Return))
        {
            ImmediateValueExpression value = new ImmediateValueExpression(new ValueToken(VKType.VOID, new DummySourcePosition("0")));
            ReturnExpression returnExpr = new ReturnExpression(new Token(TokenType.Return, new DummySourcePosition("return")), value, this);
            Expressions.Add(returnExpr);
            returnExpr.GenerateCode(gen);
        }

        return last;
    }

}