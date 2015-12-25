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
            this.RouteDisplay = new System.Windows.Forms.ListView();
            this.FirstLocation = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Profit = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Cost = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Efficiency = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.panel1 = new System.Windows.Forms.Panel();
            this.SearchRadiusInput = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.MaxPriceAgeInput = new System.Windows.Forms.NumericUpDown();
            this.MinProfitPerUnitInput = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.panel3 = new System.Windows.Forms.Panel();
            this.label5 = new System.Windows.Forms.Label();
            this.CurrentLocationLabel = new System.Windows.Forms.Label();
            this.SearchLocationsButton = new System.Windows.Forms.Button();
            this.panel2 = new System.Windows.Forms.Panel();
            this.cbAvoidPermitSystems = new System.Windows.Forms.CheckBox();
            this.LadenJumpDistanceInput = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.cbAvoidLoops = new System.Windows.Forms.CheckBox();
            this.cbOnlyLargeLanding = new System.Windows.Forms.CheckBox();
            this.CargoCapacityInput = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.InitialCreditsInput = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.RequiredStopsInput = new System.Windows.Forms.NumericUpDown();
            this.SearchButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.OptimizeProfitRadio = new System.Windows.Forms.RadioButton();
            this.OptimizeEfficiencyRadio = new System.Windows.Forms.RadioButton();
            this.cbAvoidPlanetaryStations = new System.Windows.Forms.CheckBox();
            this.statusStrip1.SuspendLayout();
            this.Tabs.SuspendLayout();
            this.Tab.SuspendLayout();
            this.tableLayoutPanel2.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.MaxPriceAgeInput)).BeginInit();
            this.panel3.SuspendLayout();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.RequiredStopsInput)).BeginInit();
            this.groupBox1.SuspendLayout();
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
            this.tableLayoutPanel2.AutoSize = true;
            this.tableLayoutPanel2.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.tableLayoutPanel2.ColumnCount = 1;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.Controls.Add(this.RouteDisplay, 0, 1);
            this.tableLayoutPanel2.Controls.Add(this.panel1, 0, 0);
            this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel2.Location = new System.Drawing.Point(3, 3);
            this.tableLayoutPanel2.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 2;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel2.Size = new System.Drawing.Size(770, 507);
            this.tableLayoutPanel2.TabIndex = 1;
            // 
            // RouteDisplay
            // 
            this.RouteDisplay.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.FirstLocation,
            this.Profit,
            this.Cost,
            this.Efficiency});
            this.RouteDisplay.Dock = System.Windows.Forms.DockStyle.Fill;
            this.RouteDisplay.FullRowSelect = true;
            this.RouteDisplay.Location = new System.Drawing.Point(3, 255);
            this.RouteDisplay.Name = "RouteDisplay";
            this.RouteDisplay.Size = new System.Drawing.Size(764, 249);
            this.RouteDisplay.TabIndex = 1;
            this.RouteDisplay.UseCompatibleStateImageBehavior = false;
            this.RouteDisplay.View = System.Windows.Forms.View.Details;
            this.RouteDisplay.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.RouteDisplay_ColumnClick);
            this.RouteDisplay.ItemActivate += new System.EventHandler(this.RouteDisplay_ItemActivate);
            // 
            // FirstLocation
            // 
            this.FirstLocation.Text = "First Location";
            this.FirstLocation.Width = 300;
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
            // panel1
            // 
            this.panel1.AutoSize = true;
            this.panel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.panel1.Controls.Add(this.SearchRadiusInput);
            this.panel1.Controls.Add(this.label7);
            this.panel1.Controls.Add(this.label8);
            this.panel1.Controls.Add(this.MaxPriceAgeInput);
            this.panel1.Controls.Add(this.MinProfitPerUnitInput);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.panel3);
            this.panel1.Controls.Add(this.panel2);
            this.panel1.Controls.Add(this.SearchButton);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(3, 3);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(764, 246);
            this.panel1.TabIndex = 2;
            // 
            // SearchRadiusInput
            // 
            this.SearchRadiusInput.Location = new System.Drawing.Point(295, 175);
            this.SearchRadiusInput.MaxLength = 64;
            this.SearchRadiusInput.Name = "SearchRadiusInput";
            this.SearchRadiusInput.Size = new System.Drawing.Size(106, 20);
            this.SearchRadiusInput.TabIndex = 17;
            this.SearchRadiusInput.Text = "70";
            this.SearchRadiusInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(213, 151);
            this.label7.Margin = new System.Windows.Forms.Padding(3);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(111, 13);
            this.label7.TabIndex = 15;
            this.label7.Text = "Max. price age (days):";
            this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(214, 178);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(75, 13);
            this.label8.TabIndex = 16;
            this.label8.Text = "Search radius:";
            // 
            // MaxPriceAgeInput
            // 
            this.MaxPriceAgeInput.Location = new System.Drawing.Point(330, 149);
            this.MaxPriceAgeInput.Maximum = new decimal(new int[] {
            365,
            0,
            0,
            0});
            this.MaxPriceAgeInput.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            -2147483648});
            this.MaxPriceAgeInput.Name = "MaxPriceAgeInput";
            this.MaxPriceAgeInput.Size = new System.Drawing.Size(73, 20);
            this.MaxPriceAgeInput.TabIndex = 15;
            this.MaxPriceAgeInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.MaxPriceAgeInput.Value = new decimal(new int[] {
            1,
            0,
            0,
            -2147483648});
            // 
            // MinProfitPerUnitInput
            // 
            this.MinProfitPerUnitInput.Location = new System.Drawing.Point(313, 123);
            this.MinProfitPerUnitInput.MaxLength = 64;
            this.MinProfitPerUnitInput.Name = "MinProfitPerUnitInput";
            this.MinProfitPerUnitInput.Size = new System.Drawing.Size(90, 20);
            this.MinProfitPerUnitInput.TabIndex = 13;
            this.MinProfitPerUnitInput.Text = "1000";
            this.MinProfitPerUnitInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(213, 126);
            this.label1.Margin = new System.Windows.Forms.Padding(3);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(94, 13);
            this.label1.TabIndex = 13;
            this.label1.Text = "Min. profit per unit:";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // panel3
            // 
            this.panel3.AutoSize = true;
            this.panel3.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.panel3.Controls.Add(this.label5);
            this.panel3.Controls.Add(this.CurrentLocationLabel);
            this.panel3.Controls.Add(this.SearchLocationsButton);
            this.panel3.Location = new System.Drawing.Point(3, 3);
            this.panel3.MinimumSize = new System.Drawing.Size(400, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(400, 42);
            this.panel3.TabIndex = 15;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(5, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(74, 13);
            this.label5.TabIndex = 9;
            this.label5.Text = "Initial location:";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // CurrentLocationLabel
            // 
            this.CurrentLocationLabel.AutoSize = true;
            this.CurrentLocationLabel.Location = new System.Drawing.Point(85, 0);
            this.CurrentLocationLabel.Name = "CurrentLocationLabel";
            this.CurrentLocationLabel.Size = new System.Drawing.Size(37, 13);
            this.CurrentLocationLabel.TabIndex = 12;
            this.CurrentLocationLabel.Text = "(none)";
            this.CurrentLocationLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // SearchLocationsButton
            // 
            this.SearchLocationsButton.AutoSize = true;
            this.SearchLocationsButton.Location = new System.Drawing.Point(8, 16);
            this.SearchLocationsButton.Name = "SearchLocationsButton";
            this.SearchLocationsButton.Size = new System.Drawing.Size(105, 23);
            this.SearchLocationsButton.TabIndex = 11;
            this.SearchLocationsButton.Text = "Search locations...";
            this.SearchLocationsButton.UseVisualStyleBackColor = true;
            this.SearchLocationsButton.Click += new System.EventHandler(this.SearchLocationsButton_Click);
            // 
            // panel2
            // 
            this.panel2.AutoSize = true;
            this.panel2.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.panel2.Controls.Add(this.cbAvoidPlanetaryStations);
            this.panel2.Controls.Add(this.cbAvoidPermitSystems);
            this.panel2.Controls.Add(this.LadenJumpDistanceInput);
            this.panel2.Controls.Add(this.label2);
            this.panel2.Controls.Add(this.cbAvoidLoops);
            this.panel2.Controls.Add(this.cbOnlyLargeLanding);
            this.panel2.Controls.Add(this.CargoCapacityInput);
            this.panel2.Controls.Add(this.label3);
            this.panel2.Controls.Add(this.InitialCreditsInput);
            this.panel2.Controls.Add(this.label4);
            this.panel2.Controls.Add(this.label6);
            this.panel2.Controls.Add(this.RequiredStopsInput);
            this.panel2.Location = new System.Drawing.Point(11, 48);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(192, 195);
            this.panel2.TabIndex = 14;
            // 
            // cbAvoidPermitSystems
            // 
            this.cbAvoidPermitSystems.AutoSize = true;
            this.cbAvoidPermitSystems.Checked = true;
            this.cbAvoidPermitSystems.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbAvoidPermitSystems.Location = new System.Drawing.Point(3, 51);
            this.cbAvoidPermitSystems.Name = "cbAvoidPermitSystems";
            this.cbAvoidPermitSystems.Size = new System.Drawing.Size(139, 17);
            this.cbAvoidPermitSystems.TabIndex = 15;
            this.cbAvoidPermitSystems.Text = "Avoid restricted systems";
            this.cbAvoidPermitSystems.UseVisualStyleBackColor = true;
            // 
            // LadenJumpDistanceInput
            // 
            this.LadenJumpDistanceInput.Location = new System.Drawing.Point(114, 146);
            this.LadenJumpDistanceInput.MaxLength = 64;
            this.LadenJumpDistanceInput.Name = "LadenJumpDistanceInput";
            this.LadenJumpDistanceInput.Size = new System.Drawing.Size(73, 20);
            this.LadenJumpDistanceInput.TabIndex = 14;
            this.LadenJumpDistanceInput.Text = "-1";
            this.LadenJumpDistanceInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(0, 149);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(108, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "Laden jump distance:";
            // 
            // cbAvoidLoops
            // 
            this.cbAvoidLoops.AutoSize = true;
            this.cbAvoidLoops.Checked = true;
            this.cbAvoidLoops.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbAvoidLoops.Location = new System.Drawing.Point(3, 6);
            this.cbAvoidLoops.Name = "cbAvoidLoops";
            this.cbAvoidLoops.Size = new System.Drawing.Size(81, 17);
            this.cbAvoidLoops.TabIndex = 5;
            this.cbAvoidLoops.Text = "Avoid loops";
            this.cbAvoidLoops.UseVisualStyleBackColor = true;
            // 
            // cbOnlyLargeLanding
            // 
            this.cbOnlyLargeLanding.AutoSize = true;
            this.cbOnlyLargeLanding.Location = new System.Drawing.Point(3, 29);
            this.cbOnlyLargeLanding.Name = "cbOnlyLargeLanding";
            this.cbOnlyLargeLanding.Size = new System.Drawing.Size(186, 17);
            this.cbOnlyLargeLanding.TabIndex = 4;
            this.cbOnlyLargeLanding.Text = "Require large landing pad stations";
            this.cbOnlyLargeLanding.UseVisualStyleBackColor = true;
            // 
            // CargoCapacityInput
            // 
            this.CargoCapacityInput.Location = new System.Drawing.Point(87, 94);
            this.CargoCapacityInput.MaxLength = 64;
            this.CargoCapacityInput.Name = "CargoCapacityInput";
            this.CargoCapacityInput.Size = new System.Drawing.Size(100, 20);
            this.CargoCapacityInput.TabIndex = 6;
            this.CargoCapacityInput.Text = "-1";
            this.CargoCapacityInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(0, 97);
            this.label3.Margin = new System.Windows.Forms.Padding(3);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(81, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Cargo capacity:";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // InitialCreditsInput
            // 
            this.InitialCreditsInput.Location = new System.Drawing.Point(74, 120);
            this.InitialCreditsInput.MaxLength = 64;
            this.InitialCreditsInput.Name = "InitialCreditsInput";
            this.InitialCreditsInput.Size = new System.Drawing.Size(113, 20);
            this.InitialCreditsInput.TabIndex = 7;
            this.InitialCreditsInput.Text = "-1";
            this.InitialCreditsInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(0, 123);
            this.label4.Margin = new System.Windows.Forms.Padding(3);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(68, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "Initial credits:";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(0, 174);
            this.label6.Margin = new System.Windows.Forms.Padding(3);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(81, 13);
            this.label6.TabIndex = 11;
            this.label6.Text = "Required stops:";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // RequiredStopsInput
            // 
            this.RequiredStopsInput.Location = new System.Drawing.Point(87, 172);
            this.RequiredStopsInput.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.RequiredStopsInput.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.RequiredStopsInput.Name = "RequiredStopsInput";
            this.RequiredStopsInput.Size = new System.Drawing.Size(100, 20);
            this.RequiredStopsInput.TabIndex = 12;
            this.RequiredStopsInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.RequiredStopsInput.Value = new decimal(new int[] {
            6,
            0,
            0,
            0});
            // 
            // SearchButton
            // 
            this.SearchButton.Location = new System.Drawing.Point(326, 217);
            this.SearchButton.Name = "SearchButton";
            this.SearchButton.Size = new System.Drawing.Size(75, 23);
            this.SearchButton.TabIndex = 8;
            this.SearchButton.Text = "Search";
            this.SearchButton.UseVisualStyleBackColor = true;
            this.SearchButton.Click += new System.EventHandler(this.SearchButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.OptimizeProfitRadio);
            this.groupBox1.Controls.Add(this.OptimizeEfficiencyRadio);
            this.groupBox1.Location = new System.Drawing.Point(209, 48);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(194, 69);
            this.groupBox1.TabIndex = 13;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Priorities";
            // 
            // OptimizeProfitRadio
            // 
            this.OptimizeProfitRadio.AutoSize = true;
            this.OptimizeProfitRadio.Location = new System.Drawing.Point(7, 44);
            this.OptimizeProfitRadio.Name = "OptimizeProfitRadio";
            this.OptimizeProfitRadio.Size = new System.Drawing.Size(118, 17);
            this.OptimizeProfitRadio.TabIndex = 1;
            this.OptimizeProfitRadio.Text = "Optimize route profit";
            this.OptimizeProfitRadio.UseVisualStyleBackColor = true;
            // 
            // OptimizeEfficiencyRadio
            // 
            this.OptimizeEfficiencyRadio.AutoSize = true;
            this.OptimizeEfficiencyRadio.Checked = true;
            this.OptimizeEfficiencyRadio.Location = new System.Drawing.Point(7, 20);
            this.OptimizeEfficiencyRadio.Name = "OptimizeEfficiencyRadio";
            this.OptimizeEfficiencyRadio.Size = new System.Drawing.Size(140, 17);
            this.OptimizeEfficiencyRadio.TabIndex = 0;
            this.OptimizeEfficiencyRadio.TabStop = true;
            this.OptimizeEfficiencyRadio.Text = "Optimize route efficiency";
            this.OptimizeEfficiencyRadio.UseVisualStyleBackColor = true;
            // 
            // cbAvoidPlanetaryStations
            // 
            this.cbAvoidPlanetaryStations.AutoSize = true;
            this.cbAvoidPlanetaryStations.Checked = true;
            this.cbAvoidPlanetaryStations.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbAvoidPlanetaryStations.Location = new System.Drawing.Point(3, 74);
            this.cbAvoidPlanetaryStations.Name = "cbAvoidPlanetaryStations";
            this.cbAvoidPlanetaryStations.Size = new System.Drawing.Size(138, 17);
            this.cbAvoidPlanetaryStations.TabIndex = 16;
            this.cbAvoidPlanetaryStations.Text = "Avoid planetary stations";
            this.cbAvoidPlanetaryStations.UseVisualStyleBackColor = true;
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
            this.Tab.PerformLayout();
            this.tableLayoutPanel2.ResumeLayout(false);
            this.tableLayoutPanel2.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.MaxPriceAgeInput)).EndInit();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.RequiredStopsInput)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.TabControl Tabs;
        private System.Windows.Forms.TabPage Tab;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.CheckBox cbOnlyLargeLanding;
        private System.Windows.Forms.CheckBox cbAvoidLoops;
        private System.Windows.Forms.TextBox CargoCapacityInput;
        private System.Windows.Forms.TextBox InitialCreditsInput;
        private System.Windows.Forms.Button SearchButton;
        private System.Windows.Forms.ListView RouteDisplay;
        private System.Windows.Forms.ColumnHeader Profit;
        private System.Windows.Forms.ColumnHeader Cost;
        private System.Windows.Forms.ColumnHeader Efficiency;
        private System.Windows.Forms.ColumnHeader FirstLocation;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button SearchLocationsButton;
        private System.Windows.Forms.Label CurrentLocationLabel;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.NumericUpDown RequiredStopsInput;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton OptimizeProfitRadio;
        private System.Windows.Forms.RadioButton OptimizeEfficiencyRadio;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox MinProfitPerUnitInput;
        private System.Windows.Forms.TextBox LadenJumpDistanceInput;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown MaxPriceAgeInput;
        private System.Windows.Forms.CheckBox cbAvoidPermitSystems;
        private System.Windows.Forms.TextBox SearchRadiusInput;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.CheckBox cbAvoidPlanetaryStations;
    }
}