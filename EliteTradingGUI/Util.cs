using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EliteTradingGUI
{
    public static class Util
    {
        public static ulong ToUlongWithInfinity(long n)
        {
            return n < 0 ? ulong.MaxValue : (ulong) n;
        }
        public static uint ToUintWithInfinity(int n)
        {
            return n < 0 ? uint.MaxValue : (uint)n;
        }

        public static byte ToByte(bool b)
        {
            return (byte) (b ? 1 : 0);
        }
    }
}
