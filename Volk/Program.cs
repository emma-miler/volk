using System.IO;
using System.Text;
using Osiris;
using Volk.Core;
using Volk.Lex;

namespace Volk;

class Program
{
    static void Main()
    {
        Log.Initialize(null);
        Log.LogDetailLevel = Log.DetailLevel.Basic;
        FileStream fs = File.OpenRead("samples/test.vk");
        List<Token> tokens = Lexer.Lex(fs).ToList();
        foreach (Token token in tokens)
        {
            Log.Info($"{token} {token.Value}");
        }

        Parser parser = new(new Queue<Token>(tokens));
        parser.Parse();
        Log.LogDetailLevel = Log.DetailLevel.None;
        foreach (Scope scope in parser.Scopes)
        {
            Log.Info($"SCOPE: {scope.Name}");
            foreach (Expression expr in scope.Expressions)
            {
                expr.Print(0);
            }
        }
        
        Log.LogDetailLevel = Log.DetailLevel.Detailed;
    }
}