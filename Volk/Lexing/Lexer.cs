using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Threading.Tasks;
using System.Transactions;
using Osiris;
using Volk.Core;
using Volk.Core.Objects;

namespace Volk.Lex;
public class Lexer
{
    int _offset = 0;
    int _length = 0;
    int _lineNumber = 0;
    int _lineOffset = 0;

    Stream _fs;
    StreamReader _data;

    VKProgram _program;

    List<SourcePosition> _lines = new();

    public Lexer(Stream fs, VKProgram program)
    {
        _fs = fs;
        _data = new StreamReader(fs);
        _program = program;
    }

    public SourcePosition GetLine(int lineNumber) => _lines[lineNumber];

    #region State Tracking

    SourcePosition GetInputTokenValue(int length = -1, bool resetLength = true)
    {
        length = length == -1 ? _length : length;
        if (resetLength)
            _length = 0;
        return new SourcePosition(_fs, _offset - length, length, _lineOffset - length, _lineNumber);
    }

    void MarkNewLine()
    {
        SourcePosition lastLine;
        if (_lines.Any())
            lastLine = _lines.Last();
        else
            lastLine = new SourcePosition(_fs, 0, 0, 0, 0);
        _lines.Add(new SourcePosition(_fs, lastLine.Offset + lastLine.Length, _lineOffset, 0, 0));
        _lineNumber++;
        _lineOffset = 0;
        _length = 0;
    }

    #endregion State Tracking

    #region Reading Functions
    char ReadByte()
    {
        _offset++;
        _length++;
        _lineOffset++;
        char c = (char)_data.Read();
        //Console.WriteLine($"{(c == '\n' ? "\\n" : c.ToString())} {dataPos.BytesRead - 1}");
        return c;
    }

    char PeekByte() => (char)_data.Peek();

    int ReadUntil(char c)
    {
        int i = 0;
        for (i = 0; _data.Peek() >= 0; i++)
        {
            if (PeekByte() == c) return i;
            ReadByte();
        }
        return i;
    }

    int ReadWhile(Predicate<char> predicate)
    {
        int i = 0;
        for (i = 0; _data.Peek() >= 0; i++)
        {
            if (!predicate(PeekByte())) return i;
            ReadByte();
        }
        return i;
    }

    #endregion Reading Functions
    public IEnumerable<Token> Lex()
    {
        char c = '\0';
        // Read until EOF
        while (_data.Peek() >= 0)
        {
            c = ReadByte();
            Log.Trace($"'{(c == '\n' ? "\\n" : c.ToString())}' (0x{((int)c).ToString("X2")})");
            if (c == '\0')
            {
                Log.Critical("Unexpected null byte!");
                Environment.Exit(1);
            }
            else if (c == ' ' || c == '\t')
            {
                _length = 0;
                Log.Trace("skipping whitespace");
                continue;
            }
            else if (c == '\n')
            {
                MarkNewLine();
                Log.Trace("skipping newline");
                continue;
            }

            // =========================
            // Dot Operator
            // =========================
            if (c == '.')
            {
                yield return new OperatorToken(OperatorType.Dot, GetInputTokenValue());
                continue;
            }

            // =========================
            // Comments
            // =========================
            if (c == '/' && PeekByte() == '/')
            {
                ReadUntil('\n');
                yield return new Token(TokenType.Comment, GetInputTokenValue());
                continue;
            }

            // =========================
            // End Of Expression
            // =========================
            if (c == ';')
            {
                yield return new Token(TokenType.EndOfExpression, GetInputTokenValue());
                continue;
            }

            // =========================
            // Name
            // =========================
            if (IsValidNameStartCharacter(c))
            {
                ReadWhile(IsValidNameCharacter);
                Token t = new Token(TokenType.Name, GetInputTokenValue(-1, false));
                TokenType? keywordType = Keyword.GetKeywordType(t.Value);
                if (keywordType != null)
                    yield return new Token(keywordType.Value, GetInputTokenValue());
                else
                {
                    _length = 0;
                    yield return t;
                }
                continue;
            }

            // =========================
            // Immediate string
            // =========================
            if (c == '"')
            {
                ReadUntil('"');
                ReadByte();
                SourcePosition newPos = new SourcePosition(_fs, _offset - _length + 1, _length - 2, _lineOffset - _length + 1, _lineNumber);
                _length = 0;
                yield return new ValueToken(VKType.STRING, newPos);
                continue;
            }

            // =========================
            // Immediate numbers
            // =========================
            if (char.IsNumber(c))
            {
                ReadWhile(IsValidNumberCharacter);
                if (PeekByte() == '.')
                {
                    ReadByte();
                    ReadWhile(IsValidNumberCharacter);
                    yield return new ValueToken(VKType.REAL, GetInputTokenValue());
                    continue;
                }
                else
                {
                    yield return new ValueToken(VKType.INT, GetInputTokenValue());
                    continue;
                }
            }

            // =========================
            // Parentheses
            // =========================
            if (c == '(')
            {
                yield return new Token(TokenType.OpenParenthesis, GetInputTokenValue(1));
                continue;
            }
            if (c == ')')
            {
                yield return new Token(TokenType.CloseParenthesis, GetInputTokenValue(1));
                continue;
            }

            // =========================
            // Brackets
            // =========================
            if (c == '{')
            {
                yield return new Token(TokenType.OpenCurlyBracket, GetInputTokenValue());
                continue;
            }
            if (c == '}')
            {
                yield return new Token(TokenType.CloseCurlyBracket, GetInputTokenValue());
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
                    yield return new OperatorToken(OperatorType.Eq, GetInputTokenValue());
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorType.Assignment, GetInputTokenValue());
                    continue;
                }
            }
            if (c == '!')
            {
                if (PeekByte() == '=')
                {

                    ReadByte();
                    yield return new OperatorToken(OperatorType.Ne, GetInputTokenValue());
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorType.Neg, GetInputTokenValue());
                    continue;
                }
            }
            if (c == '<')
            {
                if (PeekByte() == '=')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorType.Le, GetInputTokenValue());
                    continue;
                }
                else if (PeekByte() == '<')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorType.ShiftLeft, GetInputTokenValue());
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorType.Lt, GetInputTokenValue());
                    continue;
                }
            }
            if (c == '>')
            {
                if (PeekByte() == '=')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorType.Ge, GetInputTokenValue());
                    continue;
                }
                else if (PeekByte() == '>')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorType.ShiftRight, GetInputTokenValue());
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorType.Gt, GetInputTokenValue());
                    continue;
                }
            }

            if (c == '&')
            {
                if (PeekByte() == '&')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorType.LogicalAnd, GetInputTokenValue());
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorType.BitwiseAnd, GetInputTokenValue());
                    continue;
                }
            }
            if (c == '|')
            {
                if (PeekByte() == '|')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorType.LogicalOr, GetInputTokenValue());
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorType.BitwiseOr, GetInputTokenValue());
                    continue;
                }
            }
            if (c == '*')
            {
                yield return new OperatorToken(OperatorType.Mul, GetInputTokenValue());
                continue;
            }
            if (c == '/')
            {
                yield return new OperatorToken(OperatorType.Div, GetInputTokenValue());
                continue;
            }
            if (c == '+')
            {
                if (PeekByte() == '+')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorType.UnaryIncrement, GetInputTokenValue());
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorType.Add, GetInputTokenValue());
                    continue;
                }
            }
            if (c == '-')
            {
                if (PeekByte() == '-')
                {
                    ReadByte();
                    yield return new OperatorToken(OperatorType.UnaryDecrement, GetInputTokenValue());
                    continue;
                }
                else
                {
                    yield return new OperatorToken(OperatorType.Sub, GetInputTokenValue());
                    continue;
                }
            }
            if (c == '%')
            {
                yield return new OperatorToken(OperatorType.Mod, GetInputTokenValue());
                continue;
            }

            // =========================
            // Command Separator
            // =========================
            if (c == ',')
            {
                yield return new Token(TokenType.CommaSeparator, GetInputTokenValue());
                continue;
            }

            // Failed to match any token predicates
            // so its a syntax error
            SourcePosition pos = GetInputTokenValue();
            Log.Error("Syntax error: failed to lex token.");
            Log.Error($"Position: line {pos.LineNumber} character {pos.LineOffset}");
            Log.Error($"Character: '{c}' (0x{((int)c).ToString("X2")})");
            throw new FormatException("Unable to lex next token");
        }
        MarkNewLine();
        SourcePosition sPos = GetInputTokenValue();
        _lines.Add(sPos);
        yield return new Token(TokenType.EOF, new DummySourcePosition("", sPos.Offset, 1, sPos.LineOffset, sPos.LineNumber));
    }


    #region Character Functions

    static bool IsValidNameStartCharacter(char c) => char.IsLetter(c) || c == '_';
    static bool IsValidNameCharacter(char c) => char.IsLetterOrDigit(c) || c == '_';
    static bool IsValidNumberCharacter(char c) => char.IsNumber(c);

    #endregion Character Functions
}