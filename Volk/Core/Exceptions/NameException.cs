using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core;
public class NameException : Exception
{
    public Token ErrorToken {get;}

    public NameException(string message, Token errorToken) : base(message)
    {
        ErrorToken = errorToken;
    }
}