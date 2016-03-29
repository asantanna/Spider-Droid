namespace Phi.UI {
  partial class LogForm {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing) {
      if (disposing && (components != null)) {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent() {
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
      this.LogSplitter = new System.Windows.Forms.SplitContainer();
      this.panel1 = new System.Windows.Forms.Panel();
      this.button1 = new System.Windows.Forms.Button();
      this.LogTree = new System.Windows.Forms.TreeView();
      this.LogChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
      ((System.ComponentModel.ISupportInitialize)(this.LogSplitter)).BeginInit();
      this.LogSplitter.Panel1.SuspendLayout();
      this.LogSplitter.Panel2.SuspendLayout();
      this.LogSplitter.SuspendLayout();
      this.panel1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.LogChart)).BeginInit();
      this.SuspendLayout();
      // 
      // LogSplitter
      // 
      this.LogSplitter.Dock = System.Windows.Forms.DockStyle.Fill;
      this.LogSplitter.Location = new System.Drawing.Point(0, 0);
      this.LogSplitter.Name = "LogSplitter";
      // 
      // LogSplitter.Panel1
      // 
      this.LogSplitter.Panel1.Controls.Add(this.panel1);
      this.LogSplitter.Panel1.Controls.Add(this.LogTree);
      // 
      // LogSplitter.Panel2
      // 
      this.LogSplitter.Panel2.Controls.Add(this.LogChart);
      this.LogSplitter.Size = new System.Drawing.Size(1008, 661);
      this.LogSplitter.SplitterDistance = 264;
      this.LogSplitter.TabIndex = 0;
      // 
      // panel1
      // 
      this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.panel1.BackColor = System.Drawing.SystemColors.Window;
      this.panel1.Controls.Add(this.button1);
      this.panel1.Location = new System.Drawing.Point(0, 608);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(265, 53);
      this.panel1.TabIndex = 1;
      this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
      // 
      // button1
      // 
      this.button1.BackColor = System.Drawing.SystemColors.GrayText;
      this.button1.Location = new System.Drawing.Point(88, 9);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(88, 35);
      this.button1.TabIndex = 0;
      this.button1.Text = "Clear Logs";
      this.button1.UseVisualStyleBackColor = false;
      this.button1.Click += new System.EventHandler(this.ClearLog_Click);
      // 
      // LogTree
      // 
      this.LogTree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.LogTree.Location = new System.Drawing.Point(3, 3);
      this.LogTree.Name = "LogTree";
      this.LogTree.Size = new System.Drawing.Size(262, 602);
      this.LogTree.TabIndex = 0;
      this.LogTree.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.LogTree_BeforeSelect);
      this.LogTree.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.LogTree_AfterSelect);
      // 
      // LogChart
      // 
      this.LogChart.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      chartArea3.Name = "ChartArea1";
      this.LogChart.ChartAreas.Add(chartArea3);
      this.LogChart.Location = new System.Drawing.Point(3, 3);
      this.LogChart.Name = "LogChart";
      this.LogChart.Size = new System.Drawing.Size(734, 655);
      this.LogChart.TabIndex = 0;
      this.LogChart.Text = "chart1";
      this.LogChart.Click += new System.EventHandler(this.LogChart_Click);
      // 
      // LogForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(1008, 661);
      this.Controls.Add(this.LogSplitter);
      this.Name = "LogForm";
      this.Text = "Phi Logs";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.OnFormClosing);
      this.LogSplitter.Panel1.ResumeLayout(false);
      this.LogSplitter.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.LogSplitter)).EndInit();
      this.LogSplitter.ResumeLayout(false);
      this.panel1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.LogChart)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.SplitContainer LogSplitter;
    private System.Windows.Forms.TreeView LogTree;
    private System.Windows.Forms.DataVisualization.Charting.Chart LogChart;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Button button1;
  }
}