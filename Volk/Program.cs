using System.IO;
using System.Text;
using Microsoft.Extensions.Logging;
using Osiris;
using Osiris.Extensions;
using Volk.Core;
using Volk.Lex;

namespace Volk;

class Program
{

    static Lexer _lexer = null!;
    static Parser _parser = null!;

    static void Main()
    {
        Log.Initialize(null);
        Log.LogDetailLevel = Log.DetailLevel.Basic;
        FileStream fs = File.OpenRead("samples/test.vk");
        _lexer = new(fs);
        List<Token> tokens = _lexer.Lex().ToList();
        foreach (Token token in tokens)
        {
            Log.Info($"{token} {token.Value}");
        }

        _parser = new(new Queue<Token>(tokens));
        try
        {
            _parser.Parse();
        }
        catch (ParseException ex)
        {
            Log.Error(ex.ErrorToken.Value);
            IndicateToken(ex.ErrorToken);
            throw;
        }
        Log.LogDetailLevel = Log.DetailLevel.None;
        foreach (Scope scope in _parser.Scopes)
        {
            Log.Info($"SCOPE: {scope.Name}");
            foreach (Expression expr in scope.Expressions)
            {
                expr.Print(0);
            }
        }

        Log.Info($"NAME RESOLUTION");
        foreach (Scope scope in _parser.Scopes)
        {
            foreach (Expression expr in scope.Expressions)
            {
                expr.ResolveNames(scope);
            }
        }
        Log.Info($"NAME RESOLUTION SUCCESFUL");

        foreach (Scope scope in _parser.Scopes)
        {
            Log.Info($"SCOPE: {scope.ChainName}");
            foreach (Expression expr in scope.Expressions)
            {
                expr.Print(0);
            }
        }
        
        Log.LogDetailLevel = Log.DetailLevel.Detailed;
    }

    static void IndicateToken(Token t)
    {
        string line = _lexer.GetLine(t.ValueSource.LineNumber).GetValue().Replace('\n', ' ');
        Log.Error(line);
        string prefix = " ".Repeat(t.ValueSource.LineOffset);
        Log.Error(prefix + "^".Repeat(t.ValueSource.Length));
    }
}