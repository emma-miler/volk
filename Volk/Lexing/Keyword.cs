using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk.Core;
public static class Keyword
{
    public static TokenType? GetKeywordType(string value)
    {
        switch (value)
        {
            case "if": return TokenType.If;
            case "else": return TokenType.Else;
            case "for": return TokenType.For;
            case "function": return TokenType.FunctionDeclaration;
            case "return": return TokenType.Return;
            case "class": return TokenType.ClassDeclaration;
            case "new": return TokenType.NewExpression;
            case "constructor": return TokenType.KeywordConstructor;
            case "static": return TokenType.KeywordStatic;
            default: return null!;
        }
    }
}