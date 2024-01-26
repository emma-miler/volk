using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core;
public class ParseException : Exception
{
    public Token ErrorToken {get;}

    public ParseException(string message, Token errorToken) : base(message)
    {
        ErrorToken = errorToken;
    }
}