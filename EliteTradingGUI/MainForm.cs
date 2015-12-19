using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using Timer = System.Windows.Forms.Timer;

namespace EliteTradingGUI
{
    public partial class MainForm : Form, IDisposable
    {
        private EdInfo _info;
        private Config _config = Config.Load();
        private EdInfo.Location _currentLocation;
        private EdInfo.Location CurrentLocation
        {
            get
            {
                return _currentLocation;
            }
            set
            {
                _currentLocation = value;
                CurrentLocationLabel.Text = CurrentLocationString;
            }
        }
        public string CurrentLocationString
        {
            get
            {
                if (CurrentLocation == null)
                    return "(none)";
                return CurrentLocation.ToString();
            }
        }

        public MainForm()
        {
            InitializeComponent();
            Shown += OnShow;
            CurrentLocation = null;
        }

        public new void Dispose()
        {
            if (_info != null)
            {
                _info.Dispose();
                _info = null;
            }
            base.Dispose();
        }

        private void OnShow(object sender, EventArgs e)
        {
            InitializeFromConfig();

            var form = this;
            Tab.Enabled = false;
            if (!EdInfo.DatabaseExists())
            {
                double maxDistance;
                ulong minProfit;
                using (var importDbForm = new ImportDbForm())
                {
                    importDbForm.ShowDialog(this);
                    if (!importDbForm.Accepted)
                    {
                        Close();
                        return;
                    }
                    maxDistance = importDbForm.MaxDistance;
                    minProfit = importDbForm.MinProfit;
                }
                var thread = new Thread(x =>
                {
                    _info = EdInfo.ImportData(form.ReportProgress);
                    _info.RecomputeAllRoutes(maxDistance, minProfit);
                    form.OnFinishedProcessing();
                });
                thread.Start(this);
            }
            else
            {
                var thread = new Thread(x =>
                {
                    _info = new EdInfo(form.ReportProgress);
                    form.OnFinishedProcessing();
                });
                thread.Start(this);
            }
        }

        private void InitializeFromConfig()
        {
            cbOnlyLargeLanding.Checked = _config.OnlyLargeLandingPad;
            cbAvoidLoops.Checked = _config.AvoidLoops;
            CargoCapacityInput.Text = _config.CargoCapacity.ToString();
            InitialCreditsInput.Text = _config.AvailableCredits.ToString();
            RequiredStopsInput.Value = _config.RequiredStops;
            switch (_config.Optimization)
            {
                case OptimizationType.OptimizeEfficiency:
                    OptimizeEfficiencyRadio.Checked = true;
                    OptimizeProfitRadio.Checked = false;
                    break;
                case OptimizationType.OptimizeProfit:
                    OptimizeEfficiencyRadio.Checked = false;
                    OptimizeProfitRadio.Checked = true;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
            MinProfitPerUnitInput.Text = _config.MinimumProfitPerUnit.ToString();
            LadenJumpDistanceInput.Text = _config.LadenJumpDistance.ToString();
        }

        private delegate void OnFinishedProcessingDelegate();

        public void OnFinishedProcessing()
        {
            if (InvokeRequired)
            {
                BeginInvoke((OnFinishedProcessingDelegate) OnFinishedProcessing);
                return;
            }
            Tab.Enabled = true;
            ReportProgress("Ready");
        }

        private delegate void ReportProgressDelegate(string s);

        public void ReportProgress(string s)
        {
            if (InvokeRequired)
            {
                BeginInvoke((ReportProgressDelegate) ReportProgress, s);
                return;
            }
            toolStripStatusLabel1.Text = s;
        }

        private void SearchButton_Click(object sender, EventArgs e)
        {
            _config.OnlyLargeLandingPad = cbOnlyLargeLanding.Checked;
            _config.AvoidLoops = cbAvoidLoops.Checked;
            if (_currentLocation == null)
            {
                MessageBox.Show("You must select a current location.", "Parsing error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (!int.TryParse(CargoCapacityInput.Text, out _config.CargoCapacity))
            {
                MessageBox.Show("The cargo capacity must be an integer.", "Parsing error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (!long.TryParse(InitialCreditsInput.Text, out _config.AvailableCredits))
            {
                MessageBox.Show("The initial credits must be an integer.", "Parsing error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            _config.RequiredStops = Convert.ToUInt32(RequiredStopsInput.Value);
            if (OptimizeEfficiencyRadio.Checked)
                _config.Optimization = OptimizationType.OptimizeEfficiency;
            else if (OptimizeProfitRadio.Checked)
                _config.Optimization = OptimizationType.OptimizeProfit;
            else
                throw new Exception("Program in invalid state.");
            if (!ulong.TryParse(MinProfitPerUnitInput.Text, out _config.MinimumProfitPerUnit))
            {
                MessageBox.Show("The minimum profit per unit must be a non-negative integer.", "Parsing error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (!double.TryParse(LadenJumpDistanceInput.Text, out _config.LadenJumpDistance))
            {
                MessageBox.Show("The laden jump distance must be a real number.", "Parsing error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            _config.Save();

            Tab.Enabled = false;
            var form = this;
            var thread = new Thread(x =>
            {
                var routes = _info.SearchRoutes(
                    _currentLocation,
                    _config.AvoidLoops,
                    _config.OnlyLargeLandingPad,
                    _config.CargoCapacity,
                    _config.AvailableCredits,
                    _config.RequiredStops,
                    _config.Optimization,
                    _config.MinimumProfitPerUnit,
                    _config.LadenJumpDistance
                );
                form.OnFinishedSearchingRoutes(routes);
            });
            thread.Start();
        }

        private void SearchLocationsButton_Click(object sender, EventArgs e)
        {
            using (var locationSearch = new LocationSearchDialog(_info))
            {
                locationSearch.ShowDialog(this);
                if (!locationSearch.Cancelled)
                {
                    CurrentLocation = locationSearch.Result;
                    CurrentLocationLabel.Text = CurrentLocationString;
                }
            }
        }

        private delegate void OnFinishedSearchingRoutesDelegate();

        private List<RouteNode> _routes;

        private void OnFinishedSearchingRoutes(List<RouteNode> routes)
        {
            if (InvokeRequired)
            {
                BeginInvoke((OnFinishedSearchingRoutesDelegate)(() => OnFinishedSearchingRoutes(routes)));
                return;
            }
            Tab.Enabled = true;
            ReportProgress("Ready");
            _routes = routes;
            PopulateListView();
        }

        private void PopulateListView()
        {
            RouteDisplay.Items.Clear();
            foreach (var routeNode in _routes.Take(100))
            {
                var item = new ListViewItem();
                item.Text = routeNode.GetNode(1).LocationString;
                item.SubItems.Add(new ListViewItem.ListViewSubItem
                {
                    Text = routeNode.AccumulatedProfit.ToString()
                });
                item.SubItems.Add(new ListViewItem.ListViewSubItem
                {
                    Text = routeNode.Cost.ToString()
                });
                item.SubItems.Add(new ListViewItem.ListViewSubItem
                {
                    Text = routeNode.Efficiency.ToString()
                });
                item.Tag = routeNode;
                RouteDisplay.Items.Add(item);
            }
        }

        private int SortByLocationString(RouteNode x, RouteNode y)
        {
            return String.Compare(x.LocationString, y.LocationString, StringComparison.InvariantCulture);
        }

        private int SortByProfit(RouteNode x, RouteNode y)
        {
            return -x.AccumulatedProfit.CompareTo(y.AccumulatedProfit);
        }

        private int SortByCost(RouteNode x, RouteNode y)
        {
            return x.Cost.CompareTo(y.Cost);
        }

        private int SortByEfficiency(RouteNode x, RouteNode y)
        {
            return -x.Efficiency.CompareTo(y.Efficiency);
        }

        private void RouteDisplay_ColumnClick(object sender, ColumnClickEventArgs e)
        {
            switch (RouteDisplay.Columns[e.Column].Text)
            {
                case "First Location":
                    _routes.Sort(SortByLocationString);
                    break;
                case "Profit":
                    _routes.Sort(SortByProfit);
                    break;
                case "Cost":
                    _routes.Sort(SortByCost);
                    break;
                case "Efficiency":
                    _routes.Sort(SortByEfficiency);
                    break;
            }

            PopulateListView();
        }

        private void RouteDisplay_ItemActivate(object sender, EventArgs e)
        {
            if (RouteDisplay.SelectedItems.Count == 0)
                return;
            var display = new RouteDisplay((RouteNode)RouteDisplay.SelectedItems[0].Tag);
            display.Show();
        }
    }
}
