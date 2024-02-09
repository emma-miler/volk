using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core.Exceptions;
public class TokenTaggedException : Exception
{
    public Token ErrorToken {get;}
    public TokenTaggedException(string message, Token token) : base(message)
    {
        ErrorToken = token;
    }
}