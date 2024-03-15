using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core;

namespace Volk.Lexing
{
    public class DummyToken : Token
    {
        public DummyToken(TokenType type, string text) : base(type, new DummySourcePosition(text))
        {
        }
    }
}