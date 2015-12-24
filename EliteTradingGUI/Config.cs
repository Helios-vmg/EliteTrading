using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace EliteTradingGUI
{
    public enum OptimizationType
    {
        OptimizeEfficiency = 1,
        OptimizeProfit = 2,
    }

    [Serializable]
    public class Config
    {
        public bool OnlyLargeLandingPad = false;
        public bool AvoidLoops = true;
        public bool AvoidPermitSystems = true;
        public int CargoCapacity = -1;
        public long AvailableCredits = -1;
        public uint RequiredStops = 6;
        public OptimizationType Optimization = OptimizationType.OptimizeEfficiency;
        public ulong MinimumProfitPerUnit = 1000;
        public double LadenJumpDistance = -1;
        public int MaxPriceAgeDays = -1;
        public double SearchRadius = 70;

        private static string GetLocation()
        {
            var location = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            location += @"\EliteTrading";
            Directory.CreateDirectory(location);
            return location + @"\Settings.xml";
        }

        public void Save()
        {
            var serializer = new XmlSerializer(typeof (Config));
            using (var file = new FileStream(GetLocation(), FileMode.Create, FileAccess.Write, FileShare.None))
                serializer.Serialize(file, this);
        }

        public static Config Load()
        {
            try
            {
                var serializer = new XmlSerializer(typeof (Config));
                using (var file = new FileStream(GetLocation(), FileMode.Open, FileAccess.Read, FileShare.Read))
                    return (Config) serializer.Deserialize(file);
            }
            catch
            {
                return new Config();
            }
        }
    }
}
