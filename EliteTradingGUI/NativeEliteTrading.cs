using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace EliteTradingGUI
{
    public static class NativeEliteTrading
    {
        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr initialize_info(ProgressCallback temp);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void destroy_info(IntPtr instance);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int database_exists();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate void ProgressCallback(string s);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr import_data(ProgressCallback callback);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void recompute_all_routes(IntPtr instance, double maxStopDistance, ulong minProfitPerUnit);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr get_suggestions(out int n, IntPtr instance, string input);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void destroy_suggestions(IntPtr p, int n);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_name(IntPtr instance, int isStation, ulong id);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void destroy_string(IntPtr instance, IntPtr ptr);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong get_system_for_station(IntPtr instance, ulong id);

        [StructLayout(LayoutKind.Sequential)]
        public struct RouteNodeInterop
        {
            public IntPtr Previous;
            public ulong StationId;
            public double DistanceToStar;
            public ulong SystemId;
            public ulong CommodityId;
            public ulong Quantity;
            public ulong ProfitPerUnit;
            public double Efficiency;
            public ulong AccumulatedProfit;
            public ulong Expenditure;
            public double Cost;
            public double Distance;
            public uint Hops;
        };


        [StructLayout(LayoutKind.Sequential)]
        public struct RouteSearchConstraints
        {
            public ulong InitialFunds;
            public ulong MinimumProfitPerUnit;
            public double LadenJumpDistance;
            public double SearchRadius;
            public uint MaxCapacity;
            public uint RequiredStops;
            public uint Optimization;
            public int MaxPriceAgeDays;
            public byte RequireLargePad;
            public byte AvoidLoops;
            public byte AvoidPermitSystems;
        };

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr search_nearby_routes(IntPtr instance, out int resultSize, ulong currentLocation, int locationIsStation, RouteSearchConstraints constraints);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void destroy_routes(IntPtr instance, IntPtr routes, int size);

        [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern string get_commodity_name(IntPtr instance, ulong commodityId);
    }
}
