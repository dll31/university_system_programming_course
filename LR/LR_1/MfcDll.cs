using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace LR
{
    public static class MfcDll
    {
        [DllImport(@"MemMappedFileFunctions.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern void mapsend(int addr, string str);
    }
}
