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

namespace EliteTradingGUI
{
    public partial class MainForm : Form, IDisposable
    {
        private EdInfo _info;
        public MainForm()
        {
            InitializeComponent();
            Shown += OnShow;
        }

        public new void Dispose()
        {
            _info.Dispose();
            base.Dispose();
        }

        private void OnShow(object sender, EventArgs e)
        {
            if (!EdInfo.DatabaseExists())
            {
                var importDbForm = new ImportDbForm();
                importDbForm.ShowDialog(this);
                var form = this;
                var thread = new Thread(x =>
                {
                    _info = EdInfo.ImportData(form.ReportProgress);
                    _info.RecomputeAllRoutes(importDbForm.MaxDistance, importDbForm.MinProfit);
                    _info.Dispose();
                    form.ReportProgress("Ready");
                });
                thread.Start(this);
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

    }
}
