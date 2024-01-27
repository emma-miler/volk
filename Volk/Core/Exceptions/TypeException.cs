using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core;
public class TypeException : Exception
{
    public Token ErrorToken {get;}

    public TypeException(string message, Token errorToken) : base(message)
    {
        ErrorToken = errorToken;
    }
}