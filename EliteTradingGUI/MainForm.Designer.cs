namespace EliteTradingGUI
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.Tabs = new System.Windows.Forms.TabControl();
            this.Tab = new System.Windows.Forms.TabPage();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.cbOnlyLargeLanding = new System.Windows.Forms.CheckBox();
            this.cbAvoidLoops = new System.Windows.Forms.CheckBox();
            this.CargoCapacityInput = new System.Windows.Forms.TextBox();
            this.InitialCreditsInput = new System.Windows.Forms.TextBox();
            this.SearchButton = new System.Windows.Forms.Button();
            this.listView1 = new System.Windows.Forms.ListView();
            this.Profit = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Cost = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Efficiency = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.FirstSystem = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.label5 = new System.Windows.Forms.Label();
            this.statusStrip1.SuspendLayout();
            this.Tabs.SuspendLayout();
            this.Tab.SuspendLayout();
            this.tableLayoutPanel2.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
            this.statusStrip1.Location = new System.Drawing.Point(0, 539);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(784, 22);
            this.statusStrip1.TabIndex = 0;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(39, 17);
            this.toolStripStatusLabel1.Text = "Ready";
            // 
            // Tabs
            // 
            this.Tabs.Controls.Add(this.Tab);
            this.Tabs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tabs.Location = new System.Drawing.Point(0, 0);
            this.Tabs.Name = "Tabs";
            this.Tabs.SelectedIndex = 0;
            this.Tabs.Size = new System.Drawing.Size(784, 539);
            this.Tabs.TabIndex = 1;
            // 
            // Tab
            // 
            this.Tab.Controls.Add(this.tableLayoutPanel2);
            this.Tab.Location = new System.Drawing.Point(4, 22);
            this.Tab.Name = "Tab";
            this.Tab.Padding = new System.Windows.Forms.Padding(3);
            this.Tab.Size = new System.Drawing.Size(776, 513);
            this.Tab.TabIndex = 0;
            this.Tab.Text = "Search Trade Routes";
            this.Tab.UseVisualStyleBackColor = true;
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.ColumnCount = 1;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.Controls.Add(this.tableLayoutPanel1, 0, 0);
            this.tableLayoutPanel2.Controls.Add(this.listView1, 0, 1);
            this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel2.Location = new System.Drawing.Point(3, 3);
            this.tableLayoutPanel2.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 2;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 150F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel2.Size = new System.Drawing.Size(770, 507);
            this.tableLayoutPanel2.TabIndex = 1;
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanel1.Controls.Add(this.label1, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.label2, 0, 2);
            this.tableLayoutPanel1.Controls.Add(this.label3, 0, 3);
            this.tableLayoutPanel1.Controls.Add(this.label4, 0, 4);
            this.tableLayoutPanel1.Controls.Add(this.cbOnlyLargeLanding, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.cbAvoidLoops, 1, 2);
            this.tableLayoutPanel1.Controls.Add(this.CargoCapacityInput, 1, 3);
            this.tableLayoutPanel1.Controls.Add(this.InitialCreditsInput, 1, 4);
            this.tableLayoutPanel1.Controls.Add(this.SearchButton, 1, 5);
            this.tableLayoutPanel1.Controls.Add(this.label5, 0, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 7;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(770, 150);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label1.Location = new System.Drawing.Point(3, 16);
            this.label1.Margin = new System.Windows.Forms.Padding(3);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(170, 14);
            this.label1.TabIndex = 0;
            this.label1.Text = "Require large landing pad stations:";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label2.Location = new System.Drawing.Point(3, 36);
            this.label2.Margin = new System.Windows.Forms.Padding(3);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(170, 14);
            this.label2.TabIndex = 1;
            this.label2.Text = "Avoid loops:";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label3.Location = new System.Drawing.Point(3, 56);
            this.label3.Margin = new System.Windows.Forms.Padding(3);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(170, 20);
            this.label3.TabIndex = 2;
            this.label3.Text = "Cargo capacity:";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label4.Location = new System.Drawing.Point(3, 82);
            this.label4.Margin = new System.Windows.Forms.Padding(3);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(170, 20);
            this.label4.TabIndex = 3;
            this.label4.Text = "Initial credits:";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // cbOnlyLargeLanding
            // 
            this.cbOnlyLargeLanding.AutoSize = true;
            this.cbOnlyLargeLanding.Location = new System.Drawing.Point(179, 16);
            this.cbOnlyLargeLanding.Name = "cbOnlyLargeLanding";
            this.cbOnlyLargeLanding.Size = new System.Drawing.Size(15, 14);
            this.cbOnlyLargeLanding.TabIndex = 4;
            this.cbOnlyLargeLanding.UseVisualStyleBackColor = true;
            // 
            // cbAvoidLoops
            // 
            this.cbAvoidLoops.AutoSize = true;
            this.cbAvoidLoops.Location = new System.Drawing.Point(179, 36);
            this.cbAvoidLoops.Name = "cbAvoidLoops";
            this.cbAvoidLoops.Size = new System.Drawing.Size(15, 14);
            this.cbAvoidLoops.TabIndex = 5;
            this.cbAvoidLoops.UseVisualStyleBackColor = true;
            // 
            // CargoCapacityInput
            // 
            this.CargoCapacityInput.Location = new System.Drawing.Point(179, 56);
            this.CargoCapacityInput.Name = "CargoCapacityInput";
            this.CargoCapacityInput.Size = new System.Drawing.Size(100, 20);
            this.CargoCapacityInput.TabIndex = 6;
            this.CargoCapacityInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // InitialCreditsInput
            // 
            this.InitialCreditsInput.Location = new System.Drawing.Point(179, 82);
            this.InitialCreditsInput.Name = "InitialCreditsInput";
            this.InitialCreditsInput.Size = new System.Drawing.Size(100, 20);
            this.InitialCreditsInput.TabIndex = 7;
            this.InitialCreditsInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // SearchButton
            // 
            this.SearchButton.Location = new System.Drawing.Point(179, 108);
            this.SearchButton.Name = "SearchButton";
            this.SearchButton.Size = new System.Drawing.Size(75, 23);
            this.SearchButton.TabIndex = 8;
            this.SearchButton.Text = "Search";
            this.SearchButton.UseVisualStyleBackColor = true;
            this.SearchButton.Click += new System.EventHandler(this.SearchButton_Click);
            // 
            // listView1
            // 
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.Profit,
            this.Cost,
            this.Efficiency,
            this.FirstSystem});
            this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView1.Location = new System.Drawing.Point(3, 153);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(764, 351);
            this.listView1.TabIndex = 1;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            // 
            // Profit
            // 
            this.Profit.Text = "Profit";
            // 
            // Cost
            // 
            this.Cost.Text = "Cost";
            // 
            // Efficiency
            // 
            this.Efficiency.Text = "Efficiency";
            // 
            // FirstSystem
            // 
            this.FirstSystem.Text = "First System";
            this.FirstSystem.Width = 300;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label5.Location = new System.Drawing.Point(3, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(170, 13);
            this.label5.TabIndex = 9;
            this.label5.Text = "Initial location:";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(784, 561);
            this.Controls.Add(this.Tabs);
            this.Controls.Add(this.statusStrip1);
            this.MinimumSize = new System.Drawing.Size(800, 600);
            this.Name = "MainForm";
            this.Text = "MainForm";
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.Tabs.ResumeLayout(false);
            this.Tab.ResumeLayout(false);
            this.tableLayoutPanel2.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.TabControl Tabs;
        private System.Windows.Forms.TabPage Tab;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.CheckBox cbOnlyLargeLanding;
        private System.Windows.Forms.CheckBox cbAvoidLoops;
        private System.Windows.Forms.TextBox CargoCapacityInput;
        private System.Windows.Forms.TextBox InitialCreditsInput;
        private System.Windows.Forms.Button SearchButton;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ColumnHeader Profit;
        private System.Windows.Forms.ColumnHeader Cost;
        private System.Windows.Forms.ColumnHeader Efficiency;
        private System.Windows.Forms.ColumnHeader FirstSystem;
        private System.Windows.Forms.Label label5;
    }
}