using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Transactions;
using Osiris;
using Volk.Core;

namespace Volk.Lex;
public static class Lexer
{

    // This is a class so that the reader functions can modify it by-reference
    class Position
    {
        public int BytesRead = 0;
    };

    public static IEnumerable<Token> Lex(Stream fs)
    {
        Position dataPos = new();
        StreamReader data = new StreamReader(fs);

        var PeekByte = () => (char)data.Peek();

        var ReadByte = () =>
        {
            dataPos.BytesRead++;
            char c = (char)data.Read();
            //Console.WriteLine($"{(c == '\n' ? "\\n" : c.ToString())} {dataPos.BytesRead - 1}");
            return c;
        };

        var ReadUntil = (char c) =>
        {
            int i = 0;
            for (i = 0; data.Peek() >= 0; i++)
            {
                if ((char)data.Peek() == c) return i;
                ReadByte();
            }
            return i;
        };

        var ReadWhile = (Predicate<char> predicate) =>
        {
            int i = 0;
            for (i = 0; data.Peek() >= 0; i++)
            {
                if (!predicate((char)data.Peek())) return i;
                ReadByte();
            }
            return i;
        };

        char c = '\0';
        int length = 0;
        int lineNumber = 0;
        int positionBeforeLastLineBreak = 0;

        // Read until EOF
        while (data.Peek() >= 0)
        {
            length = 1;
            c = ReadByte();
            Log.Trace($"'{(c == '\n' ? "\\n" : c.ToString())}' (0x{((int)c).ToString("X2")})");
            if (c == '\0')
            {
                Log.Critical("Unexpected null byte!");
                Environment.Exit(1);
            }
            else if (c == ' ' || c == '\t')
            {
                Log.Trace("skipping whitespace");
                continue;
            }
            else if (c == '\n')
            {
                positionBeforeLastLineBreak = dataPos.BytesRead;
                lineNumber++;
                Log.Trace("skipping newline");
                continue;
            }

            // =========================
            // Comments
            // =========================
            if (c == '/' && PeekByte() == '/')
            {
                length += ReadUntil('\n');
                yield return new Token(TokenType.Comment, fs, dataPos.BytesRead, length);
                continue;
            }

            // =========================
            // End Of Expression
            // =========================
            if (c == ';')
            {
                yield return new Token(TokenType.EndOfExpression, fs, dataPos.BytesRead, length);
                continue;
            }

            // =========================
            // Name
            // =========================
            if (IsValidNameStartCharacter(c))
            {
                // TODO: keywords
                length += ReadWhile(IsValidNameCharacter);
                yield return new Token(TokenType.Name, fs, dataPos.BytesRead, length);
                continue;
            }

            // =========================
            // Immediate string
            // =========================
            if (c == '"')
            {
                length += ReadUntil('"');
                ReadByte();
                SourcePosition pos = new(dataPos.BytesRead + 1, length -1);
                yield return new ValueToken(ValueTokenType.String, pos.GetValue(fs), dataPos.BytesRead + 1, length - 1);
                continue;
            }

            // =========================
            // Immediate numbers
            // =========================
            if (char.IsNumber(c))
            {
                length += ReadWhile(IsValidNumberCharacter);
                if (PeekByte() == '.')
                {
                    ReadByte();
                    length++;
                    length += ReadWhile(IsValidNumberCharacter);
                    yield return new ValueToken(ValueTokenType.Real, fs, dataPos.BytesRead, length);
                    continue;
                }
                else
                {
                    yield return new ValueToken(ValueTokenType.Int, fs, dataPos.BytesRead, length);
                    continue;
                }
            }

            // =========================
            // Parentheses
            // =========================
            if (c == '(')
            {
                yield return new Token(TokenType.OpenParenthesis, fs, dataPos.BytesRead, 1);
                continue;
            }
            if (c == ')')
            {
                yield return new Token(TokenType.CloseParenthesis, fs, dataPos.BytesRead, 1);
                continue;
            }

            // =========================
            // Brackets
            // =========================
            if (c == '{')
            {
                yield return new Token(TokenType.OpenCurlyBracket, fs, dataPos.BytesRead, 1);
                continue;
            }
            if (c == '}')
            {
                yield return new Token(TokenType.CloseCurlyBracket, fs, dataPos.BytesRead, 1);
                continue;
            }

            // =========================
            // Brackets and Operators
            // =========================

            if (c == '<' && PeekByte() != '=')
            {
                yield return new Token(TokenType.OpenAngleBracket, fs, dataPos.BytesRead, 1);
                continue;
            }
            if (c == '>' && PeekByte() != '=')
            {
                yield return new Token(TokenType.CloseAngleBracket, fs, dataPos.BytesRead, 1);
                continue;
            }

            // =========================
            // Operators
            // =========================
            if (c == '=')
            {
                if (PeekByte() == '=')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorTokenType.Eq, fs, dataPos.BytesRead, 2);
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorTokenType.Assignment, fs, dataPos.BytesRead, 1);
                    continue;
                }
            }
            if (c == '!')
            {
                if (PeekByte() == '=')
                {

                    ReadByte();
                    yield return new OperatorToken(OperatorTokenType.Ne, fs, dataPos.BytesRead, 2);
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorTokenType.Negate, fs, dataPos.BytesRead, 2);
                    continue;
                }
            }
            if (c == '<' && PeekByte() == '=')
            {
                ReadByte();
                yield return new OperatorToken(OperatorTokenType.Le, fs, dataPos.BytesRead, 2);
                continue;
            }
            if (c == '>' && PeekByte() == '=')
            {
                ReadByte();
                yield return new OperatorToken(OperatorTokenType.Ge, fs, dataPos.BytesRead, 2);
                continue;
            }

            if (c == '&')
            {
                if (PeekByte() == '&')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorTokenType.LogicalAnd, fs, dataPos.BytesRead, 2);
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorTokenType.BitwiseAnd, fs, dataPos.BytesRead, 1);
                    continue;
                }
            }
            if (c == '|')
            {
                if (PeekByte() == '|')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorTokenType.LogicalOr, fs, dataPos.BytesRead, 2);
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorTokenType.BitwiseOr, fs, dataPos.BytesRead, 1);
                    continue;
                }
            }
            if (c == '*')
            {
                yield return new OperatorToken(OperatorTokenType.Multiply, fs, dataPos.BytesRead, 1);
                continue;
            }
            if (c == '/')
            {
                yield return new OperatorToken(OperatorTokenType.Divide, fs, dataPos.BytesRead, 1);
                continue;
            }
            if (c == '+')
            {
                yield return new OperatorToken(OperatorTokenType.Plus, fs, dataPos.BytesRead, 1);
                continue;
            }
            if (c == '-')
            {
                yield return new OperatorToken(OperatorTokenType.Minus, fs, dataPos.BytesRead, 1);
                continue;
            }
            if (c == '%')
            {
                yield return new OperatorToken(OperatorTokenType.Modulo, fs, dataPos.BytesRead, 1);
                continue;
            }

            // =========================
            // Command Separator
            // =========================
            if (c == ',')
            {
                yield return new Token(TokenType.CommaSeparator, fs, dataPos.BytesRead, 1);
                continue;
            }

            // Failed to match any token predicates
            // so its a syntax error
            Log.Error("Syntax error: failed to lex token.");
            Log.Error($"Position: line {lineNumber} character {dataPos.BytesRead - positionBeforeLastLineBreak}");
            Log.Error($"Character: '{c}' (0x{((int)c).ToString("X2")})");
            throw new FormatException("Unable to lex next token");
        }
        yield return new Token(TokenType.EOF, fs, dataPos.BytesRead + 2, 0);
    }


    #region Character Functions

    static bool IsValidNameStartCharacter(char c) => char.IsLetter(c) || c == '_';
    static bool IsValidNameCharacter(char c) => char.IsLetterOrDigit(c) || c == '_';
    static bool IsValidNumberCharacter(char c) => char.IsNumber(c);

    #endregion Character Functions
}