using System;
using System.Windows.Forms;

namespace EliteTradingGUI
{
    public partial class Form1 : Form, IDisposable
    {
        public Form1()
        {
            InitializeComponent();
            _info = new EdInfo();
        }

        public void Dispose()
        {
            _info.Dispose();
            if (_timer != null)
            {
                _timer.Stop();
                _timer.Dispose();
            }
        }

        private EdInfo _info;
        private Timer _timer;

        private void comboBox1_TextChanged(object sender, EventArgs e)
        {
            /*
            if (_timer != null)
            {
                _timer.Stop();
                _timer.Dispose();
            }
            _timer = new Timer();
            _timer.Interval = 1000;
            _timer.Tick += (x, y) =>
            {
                _timer.Stop();
                _timer.Dispose();
                _timer = null;
                var list = _info.GetSuggestions(comboBox1.Text);
                listView1.Items.Clear();
                foreach (var s in list)
                    listView1.Items.Add(s);
            };
            _timer.Start();
            */
        }
    }
}
