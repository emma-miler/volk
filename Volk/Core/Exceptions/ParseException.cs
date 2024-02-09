using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Volk.Core.Exceptions;

namespace Volk.Core;
public class ParseException : TokenTaggedException
{
    public ParseException(string message, Token token) : base(message, token) {}
}