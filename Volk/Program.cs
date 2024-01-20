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
        FileStream fs = File.OpenRead("samples/test.vk");
        List<Token> tokens = Lexer.Lex(fs).ToList();
        foreach (Token token in tokens)
        {
            Log.Info($"{token} {token.Value}");
        }

        Parser parser = new(new Queue<Token>(tokens));
        parser.Parse();
        Log.LogDetailLevel = Log.DetailLevel.None;
        foreach (Expression expr in parser.Expressions)
        {
            expr.Print(0);
        }
        Log.LogDetailLevel = Log.DetailLevel.Detailed;
    }
}