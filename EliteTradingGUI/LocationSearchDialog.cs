using System;
using System.Linq;
using System.Windows.Forms;

namespace EliteTradingGUI
{
    internal partial class LocationSearchDialog : Form, IDisposable
    {
        public LocationSearchDialog(EdInfo info)
        {
            InitializeComponent();
            _info = info;
        }

        public new void Dispose()
        {
            if (_timer != null)
            {
                _timer.Stop();
                _timer.Dispose();
            }
            base.Dispose();
        }

        private EdInfo _info;
        private Timer _timer;

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (_timer != null)
            {
                _timer.Stop();
                _timer.Dispose();
            }
            _timer = new Timer();
            _timer.Interval = 1000;
            _timer.Tick += (x, y) =>
            {
                HandleEnter();
            };
            _timer.Start();
        }

        private void HandleEnter()
        {
            if (_timer != null)
            {
                _timer.Stop();
                _timer.Dispose();
                _timer = null;
            }
            var list = _info.GetSuggestions(LocationInput.Text);
            SuggestionsDisplay.Items.Clear();
            foreach (var suggestion in list.Take(20))
            {
                var item = new ListViewItem
                {
                    Text = suggestion.ToString(),
                    Tag = suggestion,
                };
                SuggestionsDisplay.Items.Add(item);
            }
        }

        private void LocationInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter || e.KeyCode == Keys.Return)
                HandleEnter();
        }

        private bool _ignoreClose;

        private void SuggestionsDisplay_ItemActivate(object sender, EventArgs e)
        {
            if (SuggestionsDisplay.SelectedItems.Count == 0)
                return;
            Result = SuggestionsDisplay.SelectedItems[0].Tag as EdInfo.Location;
            Cancelled = false;
            _ignoreClose = true;
            try
            {
                Close();
            }
            finally
            {
                _ignoreClose = false;
            }
        }

        public EdInfo.Location Result { get; private set; }
        public bool Cancelled { get; private set; }

        private void LocationSearchDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (_ignoreClose)
                return;
            Cancelled = true;
        }

    }
}
