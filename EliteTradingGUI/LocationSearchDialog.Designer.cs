namespace EliteTradingGUI
{
    partial class LocationSearchDialog
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
            this.SuggestionsDisplay = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.tableLayout = new System.Windows.Forms.TableLayoutPanel();
            this.LocationInput = new System.Windows.Forms.TextBox();
            this.tableLayout.SuspendLayout();
            this.SuspendLayout();
            // 
            // SuggestionsDisplay
            // 
            this.SuggestionsDisplay.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
            this.SuggestionsDisplay.Dock = System.Windows.Forms.DockStyle.Fill;
            this.SuggestionsDisplay.FullRowSelect = true;
            this.SuggestionsDisplay.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
            this.SuggestionsDisplay.Location = new System.Drawing.Point(3, 29);
            this.SuggestionsDisplay.Name = "SuggestionsDisplay";
            this.SuggestionsDisplay.Size = new System.Drawing.Size(527, 498);
            this.SuggestionsDisplay.TabIndex = 1;
            this.SuggestionsDisplay.UseCompatibleStateImageBehavior = false;
            this.SuggestionsDisplay.View = System.Windows.Forms.View.Details;
            this.SuggestionsDisplay.ItemActivate += new System.EventHandler(this.SuggestionsDisplay_ItemActivate);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Width = 500;
            // 
            // tableLayout
            // 
            this.tableLayout.ColumnCount = 1;
            this.tableLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayout.Controls.Add(this.SuggestionsDisplay, 0, 1);
            this.tableLayout.Controls.Add(this.LocationInput, 0, 0);
            this.tableLayout.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayout.Location = new System.Drawing.Point(0, 0);
            this.tableLayout.Name = "tableLayout";
            this.tableLayout.RowCount = 2;
            this.tableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayout.Size = new System.Drawing.Size(533, 530);
            this.tableLayout.TabIndex = 2;
            // 
            // LocationInput
            // 
            this.LocationInput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LocationInput.Location = new System.Drawing.Point(3, 3);
            this.LocationInput.Name = "LocationInput";
            this.LocationInput.Size = new System.Drawing.Size(527, 20);
            this.LocationInput.TabIndex = 0;
            this.LocationInput.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            this.LocationInput.KeyDown += new System.Windows.Forms.KeyEventHandler(this.LocationInput_KeyDown);
            // 
            // LocationSearchDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(533, 530);
            this.Controls.Add(this.tableLayout);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "LocationSearchDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.LocationSearchDialog_FormClosing);
            this.tableLayout.ResumeLayout(false);
            this.tableLayout.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView SuggestionsDisplay;
        private System.Windows.Forms.TableLayoutPanel tableLayout;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.TextBox LocationInput;
    }
}

