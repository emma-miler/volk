using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Volk;
public static class RuntimeConfig
{
    public static int IRVerbosity = 0;

    public static void Initialize(string[] args)
    {
        for (int i = 0; i < args.Length; i++)
        {
            string arg = args[i];
            if (arg == "--ir_verbosity")
            {
                i++;
                arg = args[i];
                IRVerbosity = int.Parse(arg);
            }
        } 
    }
}