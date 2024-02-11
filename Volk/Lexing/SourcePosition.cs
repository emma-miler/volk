using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Volk.Core;

public record SourcePosition
{
    public Stream _stream;

    public int Offset { get; }
    public int Length { get; }
    public int LineOffset { get; }
    public int LineNumber { get; }

    public SourcePosition(Stream stream, int offset, int length, int lineOffset, int lineNumber)
    {
        _stream = stream;
        Offset = offset;
        Length = length;
        LineOffset = lineOffset;
        LineNumber = lineNumber;
    }

    public override string ToString()
    {
        return $"<{LineNumber}:{LineOffset}:{Length}>";
    }

    public virtual string GetValue(Encoding? encoding = null)
    {
        if (encoding == null) encoding = Encoding.ASCII;
        _stream.Seek(Offset, SeekOrigin.Begin);
        byte[] buf = new byte[Length];
        _stream.Read(buf, 0, Length);
        _stream.Seek(-Length, SeekOrigin.Current);
        return encoding.GetString(buf);
    }
}

public record DummySourcePosition : SourcePosition
{

    string _value;

    public DummySourcePosition(string value, int offset = 0, int length = 0, int lineOffset = 0, int lineNumber = 0) : base(null!, offset, length, lineOffset, lineNumber)
    {
        _value = value;
    }

    public override string GetValue(Encoding? encoding = null)
    {
        return _value;
    }
}