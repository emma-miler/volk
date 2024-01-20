using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Volk.Core;
public record struct SourcePosition
{
    public int Offset { get; }
    public int Length { get; }

    public SourcePosition(int offset, int length)
    {
        Offset = offset;
        Length = length;
    }

    public override string ToString()
    {
        return $"{{{Offset}, {Length}}}";
    }

    public string GetValue(Stream stream, Encoding? encoding = null)
    {
        if (encoding == null) encoding = Encoding.ASCII;
        stream.Seek(Offset, SeekOrigin.Begin);
        byte[] buf = new byte[Length];
        stream.Read(buf, 0, Length);
        stream.Seek(-Length, SeekOrigin.Current);
        return encoding.GetString(buf);
    }
}