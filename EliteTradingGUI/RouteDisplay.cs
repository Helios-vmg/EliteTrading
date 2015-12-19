using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EliteTradingGUI
{
    public partial class RouteDisplay : Form
    {
        public RouteDisplay(RouteNode route)
        {
            InitializeComponent();

            webBrowser.DocumentText = ToHtml(route);
        }

        private static string ToHtml(RouteNode head)
        {
            var route = head.ToList();
            var ret = new StringBuilder();
            ret.Append("<html><body>\n");
            ret.Append("<ol>\n");

            ret.Append(string.Format("<li>Start at {0}</li>\n", route[0].LocationString));
            for (int i = 1; i < route.Count; i++)
            {
                var node = route[i];
                var nextNode = route.Count > i + 1 ? route[i + 1] : null;
                ret.Append(string.Format("<li>Travel {0} ly (minimum {1} hops)</li>\n", node.Distance, node.Hops));
                ret.Append("<li>\n");
                ret.Append(string.Format("At {0} ({1} ls from star):\n", node.LocationString, node.DistanceToStar));

                ret.Append("<ul>\n");
                if (node.CommodityName != null)
                    ret.Append(string.Format("<li>\nSell all cargo. Accumulated profit: <strong>{0} Cr.</strong></li>\n", node.AccumulatedProfit));
                if (nextNode != null)
                    ret.Append(string.Format("<li>Buy {0}x {1} (expenditure: {2} Cr.)</li>\n", nextNode.Quantity, nextNode.CommodityName, nextNode.Expenditure));

                ret.Append("</li>\n");
                ret.Append("</ul>\n");

                ret.Append("</li>\n");
            }

            ret.Append("</ol>\n");
            ret.Append("</body></html>\n");
            return ret.ToString();
        }
    }
}
