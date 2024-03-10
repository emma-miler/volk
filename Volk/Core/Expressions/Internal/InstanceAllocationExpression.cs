using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;
using Volk.Core.Objects;

namespace Volk.Core.Expressions.Internal
{
    public class InstanceAllocationExpression : ValueExpression
    {

        public InstanceAllocationExpression(Token token, VKType type) : base(ValueExpressionType.Allocation, token)
        {
            ValueType = type;
        }

        public override void Print(int depth)
        {
            string prefix = " ".Repeat(depth);
            Log.Info($"{prefix}[InstanceAllocationExpression] '{ValueType}'");
        }
        
        public override void ResolveNames(VKScope scope) {}
        public override void TypeCheck(VKScope scope) {}
        public override IRVariable GenerateCode(CodeGenerator gen)
        {
            IRVariable ret = gen.NewVariable(ValueType!);
            int size = ValueType!.Fields.Count() * 8;
            gen.Operation($"{ret.Reference} = call noalias ptr @malloc(i64 noundef {size})");
            gen.Operation($"call void @llvm.memset.p0.i64({ret}, i8 0, i64 {size}, i1 false)");
            return ret;
        }

    }
}