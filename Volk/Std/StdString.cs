using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core;
using Volk.Core.Expressions;
using Volk.Core.Expressions.Internal;
using Volk.Core.Objects;
using Volk.Lexing;

namespace Volk.Std
{
    public class StdString
    {
        public static void Add(VKProgram program)
        {
            VKType.STRING = new VKType("string", true, program.RootScope);
            VKType.STRING.Fields.Add(new VKField(new DummyToken(TokenType.Name, "_str"), "_str", VKType.SYSTEM_C_STRING, 0));
            VKType.STRING.Fields.Add(new VKField(new DummyToken(TokenType.Name, "_size"), "_size", VKType.INT, 1));

            AddAllocator(program);
            AddConstructor(program);
            AddDecayToCstring(program);

            program.RootScope.Expressions.Add(new ClassDeclarationExpression(Token.Empty, VKType.STRING));
            program.RootScope.AddType(VKType.STRING);
        }

        static void AddAllocator(VKProgram program)
        {
            Token t = new DummyToken(TokenType.Name, "");
            VKFunction allocFunc = new VKFunction(VKType.STRING, "__allocate", VKType.STRING, true, new VKType[] {});
            // Add allocator call
            allocFunc.Scope.Expressions.Add(
                new ReturnExpression(
                    t, 
                    new InstanceAllocationExpression(t, VKType.STRING), 
                    allocFunc.Scope
                )
            );
            VKType.STRING.AddFunction(allocFunc);
            program.RootScope.AddFunction(allocFunc);
            program.Functions.Add(allocFunc);
        }

        static void AddDecayToCstring(VKProgram program)
        {
            VKFunction cast = new VKFunction(
                VKType.STRING, 
                "__cast", 
                VKType.SYSTEM_C_STRING, 
                false,
                new VKObject[] {
                    new VKObject("source", VKType.STRING),
            });
            cast.Scope.Expressions.Add(
                new ReturnExpression(
                    Token.Empty,
                    new DotValueExpression(
                        Token.Empty,
                        new IndirectValueExpression(new DummyToken(TokenType.Name, "source")),
                        new DummyToken(TokenType.Name, "_str")
                    ),
                    cast.Scope
                )
            );
            VKType.STRING.AddFunction(cast);
            program.RootScope.AddFunction(cast);
            program.Functions.Add(cast);
        }

        static void AddConstructor(VKProgram program)
        {
            VKFunction constructor = new VKFunction(
                VKType.STRING, 
                "__new", 
                VKType.STRING, 
                false,
                new VKObject[] {
                    new VKObject("this", VKType.STRING),
                    new VKObject("str", VKType.SYSTEM_C_STRING),
                    new VKObject("size", VKType.INT),
            });
            constructor.Scope.Expressions.Add(
                new AssignmentExpression(
                    Token.Empty,
                    new DotValueExpression(
                        Token.Empty,
                        new IndirectValueExpression(new DummyToken(TokenType.Name, "this")),
                        new DummyToken(TokenType.Name, "_str")
                    ),
                    new IndirectValueExpression(new DummyToken(TokenType.Name, "str"))
                )
            );
            constructor.Scope.Expressions.Add(
                new AssignmentExpression(
                    Token.Empty,
                    new DotValueExpression(
                        Token.Empty,
                        new IndirectValueExpression(new DummyToken(TokenType.Name, "this")),
                        new DummyToken(TokenType.Name, "_size")
                    ),
                    new IndirectValueExpression(new DummyToken(TokenType.Name, "size"))
                )
            );
            constructor.Scope.Expressions.Add(
                new ReturnExpression(
                    new DummyToken(TokenType.Return, "return"),
                    new IndirectValueExpression(new DummyToken(TokenType.Name, "this")),
                    constructor.Scope
                )
            );
            VKType.STRING.AddFunction(constructor);
            program.RootScope.AddFunction(constructor);
            program.Functions.Add(constructor);
        }
    }
}