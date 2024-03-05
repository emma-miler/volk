using System.IO;
using System.Text;
using Microsoft.Extensions.Logging;
using Osiris;
using Osiris.Extensions;
using Volk.Core;
using Volk.Core.Exceptions;
using Volk.Core.Objects;
using Volk.Lex;

namespace Volk;

class App
{

    static Lexer _lexer = null!;
    static Parser _parser = null!;

    static void Main(string[] args)
    {
        Log.Initialize(null);
        Log.LogDetailLevel = Log.DetailLevel.Basic;
        RuntimeConfig.Initialize(args);

        if (args.Length == 0)
        {
            Log.Error("Please provide a filename to compile as the first argument");
            return;
        }

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
        catch (TokenTaggedException ex)
        {
            Log.Error(ex.ErrorToken.Value);
            IndicateToken(ex.ErrorToken);
            throw;
        }

        
        program.PrintExpressions();

        Log.Info($"START NAME RESOLUTION AND TYPE CHECK");
        try{
        foreach (VKFunction function in program.Functions)
        {
            foreach (Expression expr in function.Scope.Expressions)
            {
                expr.ResolveNames(function.Scope);
            }
            foreach (Expression expr in function.Scope.Expressions)
            {
                expr.TypeCheck(function.Scope);
            }
        }
        }
        catch (TokenTaggedException ex)
        {
            if (ex.ErrorToken != null)
                IndicateToken(ex.ErrorToken);
            throw;
        }
        Log.Info($"END NAME RESOLUTION AND TYPE CHECK");

        program.PrintExpressions();

        List<string> output = new();
        CodeGenerator gen = new();
        gen.AddStringTable(program.CompileTimeStrings);
        foreach (VKFunction function in program.Functions)
        {
           function.GenerateCode(gen);
        }
        output.AddRange(gen.Lines);
        List<string> consoleOutput = output.Zip(Enumerable.Range(0, output.Count))
        .Select((line, num) => (num+1).ToString().PadLeft(3, '0') + " " + line.First)
        .ToList();

        Log.LogDetailLevel = Log.DetailLevel.None;
        foreach (string line in consoleOutput)
        {
            Log.Custom(LogLevel.Information, line, null, line.Contains(";") ? ConsoleColor.DarkGray : ConsoleColor.Green);
        }
        
        Log.LogDetailLevel = Log.DetailLevel.Detailed;
        string newFileName = args[0];
        newFileName = newFileName.ReplaceLast(".vk", ".ll");
        string outputString = string.Join('\n', output); 
        File.WriteAllText(newFileName, outputString);
    }

    static void IndicateToken(Token t)
    {
        string line = _lexer.GetLine(t.ValueSource.LineNumber).GetValue().Replace('\n', ' ');
        Log.Error(line);
        string prefix = " ".Repeat(t.ValueSource.LineOffset);
        Log.Error(prefix + "^".Repeat(t.ValueSource.Length));
        Log.Error($"Line: {t.ValueSource.LineNumber}, Offset: {t.ValueSource.Offset}, Token Length: {t.Value.Length}");
    }
}