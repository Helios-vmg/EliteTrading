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

        public class Location
        {
            public readonly EdInfo Info;
            public bool IsStation;
            public ulong Id;
            private string _name;
            private string _systemName;

            internal Location(EdInfo info)
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

        public List<Location> GetSuggestions(string s)
        {
            int returnedSuggestions;
            var suggestions = NativeEliteTrading.get_suggestions(out returnedSuggestions, _instance, s);
            try
            {
                var ret = new List<Location>();
                ret.Capacity = returnedSuggestions;
                for (int i = 0; i < returnedSuggestions; i++)
                {
                    var id = Marshal.ReadInt64(suggestions, i*8);
                    ret.Add(
                        new Location(this)
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

        private List<RouteNode> SearchRoutes(bool isStation, ulong id, bool avoidLoops, bool requireLargePad, int cargoCapacity, long initialCredits, uint requiredStops, OptimizationType optimization, ulong minimumProfitPerUnit, double ladenJumpDistance)
        {
            int size;
            SearchFlags flags = SearchFlags.None;
            if (isStation)
                flags |= SearchFlags.CurrentLocationIsStation;
            if (avoidLoops)
                flags |= SearchFlags.AvoidLoops;
            if (requireLargePad)
                flags |= SearchFlags.RequireLargePad;
            var routes = NativeEliteTrading.search_nearby_routes(
                _instance,
                out size,
                id,
                (uint)flags,
                cargoCapacity,
                initialCredits,
                requiredStops,
                (int)optimization,
                minimumProfitPerUnit,
                ladenJumpDistance
            );
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

        public List<RouteNode> SearchRoutes(Location currentLocation, bool avoidLoops, bool requireLargePad, int cargoCapacity, long initialCredits, uint requiredStops, OptimizationType optimization, ulong minimumProfitPerUnit, double ladenJumpDistance)
        {
            return SearchRoutes(currentLocation.IsStation, currentLocation.Id, avoidLoops, requireLargePad, cargoCapacity, initialCredits, requiredStops, optimization, minimumProfitPerUnit, ladenJumpDistance);
        }

        public string GetCommodityName(ulong commodityId)
        {
            return NativeEliteTrading.get_commodity_name(_instance, commodityId);
        }
    }
}
