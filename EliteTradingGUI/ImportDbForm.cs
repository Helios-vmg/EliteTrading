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
    public partial class ImportDbForm : Form
    {
        public ImportDbForm()
        {
            InitializeComponent();
            var tooltip = new ToolTip();
            tooltip.SetToolTip(maxRouteStopDistanceInput, "This value is used " + 
                "to generate trading route segments. Route segments between " +
                "stations farther than this value will be ignored. If it's " +
                "later desired to search for routes with longer segments, the " +
                "segments will have to be recomputed. If unsure, leave it as " +
                "it is.");
            tooltip = new ToolTip();
            tooltip.SetToolTip(minProfitPerUnitInput, "This value is used " + 
                "to generate trading route segments. Route segments that " +
                "involve trades less profitable that this value per unit sold " +
                "will be ignored. It's recommended to leave the default value. " +
                "Setting a very low minimum may result in a very large database " +
                "and diminished performance during route searches.");
            
        }

        private void OkButton_Click(object sender, EventArgs e)
        {
            if (!double.TryParse(maxRouteStopDistanceInput.Text, out _maxDistance))
            {
                MessageBox.Show("The maximum route stop distance must be a real value.", "Error", MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }
            if (!ulong.TryParse(minProfitPerUnitInput.Text, out _minProfit))
            {
                MessageBox.Show("The minimum profit per unit must be a non-negative integer.", "Error", MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }
            Close();
        }

        private double _maxDistance;
        private ulong _minProfit;

        public double MaxDistance
        {
            get { return _maxDistance; }
        }

        public ulong MinProfit
        {
            get { return _minProfit; }
        }
    }
}
