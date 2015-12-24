using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace EliteTradingGUI
{
    public class RouteNode
    {
        public EdInfo Info;
        public RouteNode Previous;
        public string StationName;
        public double DistanceToStar;
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
        public double Distance;
        public uint Hops;
        public readonly List<ulong> HopRoute;

        public RouteNode(EdInfo info, IntPtr ptr)
        {
            var structure = (NativeEliteTrading.RouteNodeInterop)Marshal.PtrToStructure(ptr, typeof(NativeEliteTrading.RouteNodeInterop));
            Info = info;
            if (structure.Previous != IntPtr.Zero)
                Previous = new RouteNode(info, structure.Previous);
            StationId = structure.StationId;
            DistanceToStar = structure.DistanceToStar;
            SystemId = structure.SystemId;
            SystemName = info.GetSystemName(SystemId);
            if (StationId != ulong.MaxValue)
                StationName = info.GetStationName(StationId);
            else
                StationName = SystemName;
            CommodityId = structure.CommodityId;
            Quantity = structure.Quantity;
            ProfitPerUnit = structure.ProfitPerUnit;
            Efficiency = structure.Efficiency;
            AccumulatedProfit = structure.AccumulatedProfit;
            Expenditure = structure.Expenditure;
            Cost = structure.Cost;
            Distance = structure.Distance;
            Hops = structure.Hops;
            HopRoute = new List<ulong>();
            HopRoute.Capacity = structure.HopRouteSize;
            if (structure.HopRoute != IntPtr.Zero)
            {
                Debug.Assert(Previous != null);
                HopRoute.Add(Previous.SystemId);
                for (int i = 0; i < structure.HopRouteSize; i++)
                    HopRoute.Add((ulong) Marshal.ReadInt64(structure.HopRoute + i*8));
            }
        }

        private string _locationString;

        public string LocationString
        {
            get
            {
                if (_locationString == null)
                    _locationString = ToLocation().ToString();
                return _locationString;
            }
        }

        private int GetNode(int i, out RouteNode result)
        {
            if (Previous == null)
            {
                if (i == 0)
                    result = this;
                else
                    result = null;
                return 0;
            }
            var index = 1 + Previous.GetNode(i, out result);
            if (index == i)
                result = this;
            return index;
        }

        public RouteNode GetNode(int i)
        {
            RouteNode ret;
            GetNode(i, out ret);
            return ret;
        }

        public EdInfo.Location ToLocation()
        {
            var ret = new EdInfo.Location(Info);
            if (StationId != ulong.MaxValue)
            {
                ret.IsStation = true;
                ret.Id = StationId;
            }
            else
            {
                ret.IsStation = false;
                ret.Id = SystemId;
            }
            return ret;
        }

        public List<RouteNode> ToList()
        {
            if (Previous == null)
                return new List<RouteNode>{this};
            var ret = Previous.ToList();
            ret.Add(this);
            return ret;
        }

        private bool _commodityNameSet = false;
        private string _commodityName;

        public bool ValidCommodityId
        {
            get { return CommodityId != ulong.MaxValue; }
        }

        public string CommodityName
        {
            get
            {
                if (!_commodityNameSet)
                {
                    if (ValidCommodityId)
                        _commodityName = Info.GetCommodityName(CommodityId);
                    _commodityNameSet = true;
                }
                return _commodityName;
            }
        }
    }
}
