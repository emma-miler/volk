using System.IO;
using System.Text;
using Microsoft.Extensions.Logging;
using Osiris;
using Osiris.Extensions;
using Volk.Core;
using Volk.Core.Objects;
using Volk.Lex;

namespace Volk;

class Program
{

    static Lexer _lexer = null!;
    static Parser _parser = null!;

    static void Main(string[] args)
    {
        Log.Initialize(null);
        Log.LogDetailLevel = Log.DetailLevel.Basic;
        FileInfo file = new FileInfo(args[0]);
        FileStream fs = File.OpenRead(args[0]);
        

        VKProgram program = new();
        _lexer = new(fs, program);
        List<Token> tokens = _lexer.Lex().ToList();
        foreach (Token token in tokens)
        {
            Log.Info($"{token} {token.Value}");
        }

        _parser = new(new Queue<Token>(tokens), program);
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

        
        program.PrintExpressions();

        Log.Info($"START NAME RESOLUTION AND TYPE CHECK");
        foreach (Scope scope in _parser.Scopes)
        {
            foreach (Expression expr in scope.Expressions)
            {
                expr.ResolveNames(scope);
            }
            foreach (Expression expr in scope.Expressions)
            {
                expr.TypeCheck(scope);
            }
        }
        Log.Info($"END NAME RESOLUTION AND TYPE CHECK");

        program.PrintExpressions();

        string output = "";
        CodeGenerator gen = new();
        gen.AddStringTable(program.CompileTimeStrings);
        foreach (VKFunction function in program.Functions)
        {
           function.GenerateCode(gen);
        }
        output += gen.Build();

        Log.LogDetailLevel = Log.DetailLevel.None;
        Log.Info(output);
        Log.LogDetailLevel = Log.DetailLevel.Detailed;
        string newFileName = args[0];
        newFileName = newFileName.ReplaceLast(".vk", ".ll");
        File.WriteAllText(newFileName, output);
        
    }

    static void IndicateToken(Token t)
    {
        string line = _lexer.GetLine(t.ValueSource.LineNumber).GetValue().Replace('\n', ' ');
        Log.Error(line);
        string prefix = " ".Repeat(t.ValueSource.LineOffset);
        Log.Error(prefix + "^".Repeat(t.ValueSource.Length));
    }
}