using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Osiris;
using Osiris.Extensions;

namespace Volk.Core.Expressions
{
    public class ArgumentPackValueExpression : ValueExpression
    {
        
        public List<ValueExpression> Expressions;

        public ArgumentPackValueExpression(Token token, List<ValueExpression> expressions) : base (ValueExpressionType.ArgumentPack, token)
        {
            Expressions = expressions;
        }

        public override void Print(int depth)
        {
            string prefix = " ".Repeat(depth);
            Log.Info($"{prefix}[ArgumentPackValueExpression]");
            foreach (Expression init in Expressions)
            {
                init.Print(depth + 2);
            }
        }

        public override IRVariable GenerateCode(CodeGenerator gen)
        {
            throw new NotImplementedException();
        }

        

        public override void ResolveNames(VKScope scope)
        {
            throw new NotImplementedException();
        }

        public override void TypeCheck(VKScope scope)
        {
            throw new NotImplementedException();
        }
    }
}