using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions.Internal
{
    public class ConstantValueExpression : ValueExpression
    {
        string _value;
        public ConstantValueExpression(Token token, VKType type, string value) : base(ValueExpressionType.Constant, token)
        {
            ValueType = type;
            _value = value;
        }

        public override void Print(int depth)
        {
            string prefix = " ".Repeat(depth);
            Log.Info($"{prefix}[ConstantValueExpression] '{_value}'");
        }

        public override void ResolveNames(VKScope scope) {}
        public override void TypeCheck(VKScope scope) {}
        public override IRVariable GenerateCode(CodeGenerator gen)
        {
            return new IRVariable(_value, ValueType!, IRVariableType.Constant);
        }
    }
}