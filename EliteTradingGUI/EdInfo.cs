﻿using System;
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

        private List<RouteNode> SearchRoutes(bool isStation, ulong id, NativeEliteTrading.RouteSearchConstraints constraints)
        {
            int size;
            var routes = NativeEliteTrading.search_nearby_routes(_instance, out size, id, isStation ? 1 : 0, constraints);
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

        public List<RouteNode> SearchRoutes(Location currentLocation, NativeEliteTrading.RouteSearchConstraints constraints)
        {
            return SearchRoutes(currentLocation.IsStation, currentLocation.Id, constraints);
        }

        public string GetCommodityName(ulong commodityId)
        {
            return NativeEliteTrading.get_commodity_name(_instance, commodityId);
        }

        public List<SystemPoint> GetSystemPointList()
        {
            int size;
            var pointer = NativeEliteTrading.get_system_point_list(_instance, out size);
            try
            {
                var type = typeof (SystemPoint);
                var structSize = Marshal.SizeOf(type);
                var ret = new List<SystemPoint>();
                ret.Capacity = size;
                for (int i = 0; i < size; i++)
                    ret.Add((SystemPoint)Marshal.PtrToStructure(pointer + i * structSize, type));
                return ret;
            }
            finally
            {
                NativeEliteTrading.destroy_system_point_list(_instance, pointer, size);
            }
        }
    }
}
