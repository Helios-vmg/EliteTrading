using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace EliteTradingGUI
{
    public class EdInfo : IDisposable
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
                public ulong SystemId;
                public ulong CommodityId;
                public ulong Quantity;
                public ulong ProfitPerUnit;
                public double Efficiency;
                public ulong AccumulatedProfit;
                public ulong Expenditure;
                public double Cost;
            };

            [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
            public static extern IntPtr search_nearby_routes(IntPtr instance, out int resultSize, ulong currentLocation,
                uint flags, int cargoCapacity, long initialCredits, uint requiredStops, int optimizationSetting,
                ulong minimumProfitPerUnit);

            [DllImport("elite_trading.dll", CallingConvention = CallingConvention.Cdecl)]
            public static extern void destroy_routes(IntPtr instance, IntPtr routes, int size);
        }
        private IntPtr _instance;
        private GCHandle _callback;
        public EdInfo(Action<string> callback)
        {
            var temp = (NativeEliteTrading.ProgressCallback)(x => callback(x));
            _callback = GCHandle.Alloc(temp);
            _instance = NativeEliteTrading.initialize_info(temp);
        }

        private EdInfo(IntPtr instance, GCHandle callback)
        {
            _instance = instance;
            _callback = callback;
        }

        public void Dispose()
        {
            NativeEliteTrading.destroy_info(_instance);
            _callback.Free();
        }

        public static bool DatabaseExists()
        {
            return NativeEliteTrading.database_exists() != 0;
        }

        public static EdInfo ImportData(Action<string> callback)
        {
            var temp = (NativeEliteTrading.ProgressCallback)(x => callback(x));
            var handle = GCHandle.Alloc(temp);
            var instance = NativeEliteTrading.import_data(temp);
            return new EdInfo(instance, handle);
        }

        public void RecomputeAllRoutes(double maxStopDistance, ulong minProfitPerUnit)
        {
            NativeEliteTrading.recompute_all_routes(_instance, maxStopDistance, minProfitPerUnit);
        }

        public class Suggestion
        {
            public readonly EdInfo Info;
            public bool IsStation;
            public ulong Id;
            private string _name;
            private string _systemName;

            internal Suggestion(EdInfo info)
            {
                Info = info;
            }

            public string Name
            {
                get
                {
                    if (_name == null)
                    {
                        if (!IsStation)
                            _systemName = _name = Info.GetSystemName(Id);
                        else
                            _name = Info.GetStationName(Id);
                    }
                    return _name;
                }
            }

            public string SystemName
            {
                get
                {
                    if (_systemName == null)
                    {
                        if (!IsStation)
                            return Name;
                        _systemName = Info.GetSystemName(Info.GetSystemForStation(Id));
                    }
                    return _systemName;
                }
            }

            public override string ToString()
            {
                if (!IsStation)
                    return "System \"" + Name + "\"";
                return "Station \"" + Name + "\" in system \"" + SystemName + "\"";
            }
        }

        public List<Suggestion> GetSuggestions(string s)
        {
            int returnedSuggestions;
            var suggestions = NativeEliteTrading.get_suggestions(out returnedSuggestions, _instance, s);
            try
            {
                var ret = new List<Suggestion>();
                ret.Capacity = returnedSuggestions;
                for (int i = 0; i < returnedSuggestions; i++)
                {
                    var id = Marshal.ReadInt64(suggestions, i*8);
                    ret.Add(
                        new Suggestion(this)
                        {
                            IsStation = id < 0,
                            Id = (ulong) Math.Abs(id) - 1,
                        }
                    );
                }
                return ret;
            }
            finally
            {
                NativeEliteTrading.destroy_suggestions(suggestions, returnedSuggestions);
            }
        }

        private string GetName(bool isStation, ulong id)
        {
            var ptr = NativeEliteTrading.get_name(_instance, isStation ? 1 : 0, id);
            if (ptr == IntPtr.Zero)
                throw new IndexOutOfRangeException("Invalid ID.");
            try
            {
                return Marshal.PtrToStringAnsi(ptr);
            }
            finally
            {
                NativeEliteTrading.destroy_string(_instance, ptr);
            }
        }

        public string GetSystemName(ulong id)
        {
            return GetName(false, id);
        }

        public string GetStationName(ulong id)
        {
            return GetName(true, id);
        }

        public ulong GetSystemForStation(ulong id)
        {
            var ret = NativeEliteTrading.get_system_for_station(_instance, id);
            if (ret == ulong.MaxValue)
                throw new IndexOutOfRangeException("Invalid ID.");
            return ret;
        }

        [Flags]
        private enum SearchFlags
        {
            None = 0,
            CurrentLocationIsStation = 1,
            AvoidLoops = 2,
            RequireLargePad = 4,
        }

        private List<RouteNode> SearchRoutes(bool isStation, ulong id, bool avoidLoops, bool requireLargePad, int cargoCapacity, long initialCredits, uint requiredStops, OptimizationType optimization, ulong minimumProfitPerUnit)
        {
            int size;
            SearchFlags flags = SearchFlags.None;
            if (isStation)
                flags |= SearchFlags.CurrentLocationIsStation;
            if (avoidLoops)
                flags |= SearchFlags.AvoidLoops;
            if (requireLargePad)
                flags |= SearchFlags.RequireLargePad;
            var routes = NativeEliteTrading.search_nearby_routes(_instance, out size, id, (uint)flags, cargoCapacity, initialCredits, requiredStops, (int)optimization, minimumProfitPerUnit);
            try
            {
                var ret = new List<RouteNode>();
                if (routes == IntPtr.Zero)
                    return ret;
                ret.Capacity = size;
                for (int i = 0; i < size; i++)
                {
                    var element = Marshal.ReadIntPtr(routes, i*IntPtr.Size);
                    ret.Add(new RouteNode(this, element));
                }
                return ret;
            }
            finally
            {
                NativeEliteTrading.destroy_routes(_instance, routes, size);
            }
        }

        public List<RouteNode> SearchRoutes(Suggestion currentLocation, bool avoidLoops, bool requireLargePad, int cargoCapacity, long initialCredits, uint requiredStops, OptimizationType optimization, ulong minimumProfitPerUnit)
        {
            return SearchRoutes(currentLocation.IsStation, currentLocation.Id, avoidLoops, requireLargePad, cargoCapacity, initialCredits, requiredStops, optimization, minimumProfitPerUnit);
        }
    }

    public class RouteNode
    {
        public EdInfo Info;
        public RouteNode Previous;
        public string StationName;
        public string SystemName;
        public ulong StationId;
        public ulong SystemId;
        public ulong CommodityId;
        public ulong Quantity;
        public ulong ProfitPerUnit;
        public double Efficiency;
        public ulong AccumulatedProfit;
        public ulong Expenditure;
        public double Cost;

        public RouteNode(EdInfo info, IntPtr ptr)
        {
            var structure = (EdInfo.NativeEliteTrading.RouteNodeInterop)Marshal.PtrToStructure(ptr, typeof(EdInfo.NativeEliteTrading.RouteNodeInterop));
            Info = info;
            if (structure.Previous != IntPtr.Zero)
                Previous = new RouteNode(info, structure.Previous);
            StationId = structure.StationId;
            SystemId = structure.SystemId;
            StationName = info.GetStationName(StationId);
            SystemName = info.GetSystemName(SystemId);
            CommodityId = structure.CommodityId;
            Quantity = structure.Quantity;
            ProfitPerUnit = structure.ProfitPerUnit;
            Efficiency = structure.Efficiency;
            AccumulatedProfit = structure.AccumulatedProfit;
            Expenditure = structure.Expenditure;
            Cost = structure.Cost;
        }
    };
}
