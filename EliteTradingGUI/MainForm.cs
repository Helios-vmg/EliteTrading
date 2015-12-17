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

        public MainForm()
        {
            InitializeComponent();
            Shown += OnShow;
        }

        public new void Dispose()
        {
            if (_info != null)
            {
                _info.Dispose();
                _info = null;
            }
            if (_timer != null)
            {
                _timer.Stop();
                _timer.Dispose();
                _timer = null;
            }
            base.Dispose();
        }

        private void OnShow(object sender, EventArgs e)
        {
            cbOnlyLargeLanding.Checked = _config.OnlyLargeLandingPad;
            cbAvoidLoops.Checked = _config.AvoidLoops;
            CargoCapacityInput.Text = _config.CargoCapacity.ToString();
            InitialCreditsInput.Text = _config.AvailableCredits.ToString();

            if (!EdInfo.DatabaseExists())
            {
                Tab.Enabled = false;
                var importDbForm = new ImportDbForm();
                importDbForm.ShowDialog(this);
                if (!importDbForm.Accepted)
                {
                    Close();
                    return;
                }
                var form = this;
                var thread = new Thread(x =>
                {
                    _info = EdInfo.ImportData(form.ReportProgress);
                    _info.RecomputeAllRoutes(importDbForm.MaxDistance, importDbForm.MinProfit);
                    Tab.Enabled = true;
                    form.ReportProgress("Ready");
                });
                thread.Start(this);
            }
            else
            {
                _info = new EdInfo();
            }
        }

        private delegate void ReportProgressDelegate(string s);

        public void ReportProgress(string s)
        {
            if (InvokeRequired)
            {
                BeginInvoke((ReportProgressDelegate)ReportProgress, s);
                return;
            }
            toolStripStatusLabel1.Text = s;
        }

        private void SearchButton_Click(object sender, EventArgs e)
        {
            _config.OnlyLargeLandingPad = cbOnlyLargeLanding.Checked;
            _config.AvoidLoops = cbAvoidLoops.Checked;
            if (!int.TryParse(CargoCapacityInput.Text, out _config.CargoCapacity))
            {
                MessageBox.Show("The cargo capacity must be an integer.", "Error", MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }
            if (!long.TryParse(InitialCreditsInput.Text, out _config.AvailableCredits))
            {
                MessageBox.Show("The initial credits must be an integer.", "Error", MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }
            _config.Save();
        }

        private Timer _timer;

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (_timer != null)
            {
                _timer.Stop();
                _timer.Dispose();
            }
            _timer = new Timer
            {
                Interval = 1000,
            };
            _timer.Tick += (x, y) =>
            {
                _timer.Stop();
                _timer.Dispose();
                _timer = null;
                var suggestions = _info.GetSuggestions(InitialLocationInput.Text);
                InitialLocationInput.AutoCompleteCustomSource.Clear();
                InitialLocationInput.AutoCompleteCustomSource.AddRange(suggestions);
                InitialLocationInput.AutoCompleteMode
            };
            _timer.Start();
        }

    }
}
