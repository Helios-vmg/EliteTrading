using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace EliteTradingGUI
{
    class EdInfo : IDisposable
    {
        private static class NativeEliteTrading
        {
            [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
            public static extern IntPtr initialize_info();

            [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
            public static extern void destroy_info(IntPtr instance);
        
            [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
            public static extern int database_exists();

            [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public delegate void ProgressCallback(string s);

            [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
            public static extern IntPtr import_data(ProgressCallback callback);

            [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
            public static extern void recompute_all_routes(IntPtr instance, double max_stop_distance, ulong min_profit_per_unit);

            [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
            public static extern IntPtr get_suggestions(out int n, IntPtr instance, string input);
            
            [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
            public static extern void destroy_suggestions(IntPtr p, int n);
        }
        private IntPtr _instance;
        public EdInfo()
        {
            _instance = NativeEliteTrading.initialize_info();
        }

        private EdInfo(IntPtr instance)
        {
            _instance = instance;
        }

        public void Dispose()
        {
            NativeEliteTrading.destroy_info(_instance);
        }

        public static bool DatabaseExists()
        {
            return NativeEliteTrading.database_exists() != 0;
        }

        public static EdInfo ImportData(Action<string> callback)
        {
            var temp = (NativeEliteTrading.ProgressCallback)(x => callback(x));
            var handle = GCHandle.Alloc(temp);
            IntPtr instance;
            try
            {
                instance = NativeEliteTrading.import_data(temp);
            }
            finally
            {
                handle.Free();
            }
            return new EdInfo(instance);
        }

        public void RecomputeAllRoutes(double maxStopDistance, ulong minProfitPerUnit)
        {
            NativeEliteTrading.recompute_all_routes(_instance, maxStopDistance, minProfitPerUnit);
        }

        public string[] GetSuggestions(string s)
        {
            int n;
            var suggestions = NativeEliteTrading.get_suggestions(out n, _instance, s);
            try
            {
                var ret = new string[n];
                for (int i = 0; i < n; i++)
                {
                    var ptr = Marshal.ReadIntPtr(suggestions, i * IntPtr.Size);
                    ret[i] = Marshal.PtrToStringAnsi(ptr);
                }
                return ret;
            }
            finally
            {
                NativeEliteTrading.destroy_suggestions(suggestions, n);
            }
        }
    }
}
