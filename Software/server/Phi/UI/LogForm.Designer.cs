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
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
      this.LogSplitter = new System.Windows.Forms.SplitContainer();
      this.LogTree = new System.Windows.Forms.TreeView();
      this.LogChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
      ((System.ComponentModel.ISupportInitialize)(this.LogSplitter)).BeginInit();
      this.LogSplitter.Panel1.SuspendLayout();
      this.LogSplitter.Panel2.SuspendLayout();
      this.LogSplitter.SuspendLayout();
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
      this.LogSplitter.Panel1.Controls.Add(this.LogTree);
      // 
      // LogSplitter.Panel2
      // 
      this.LogSplitter.Panel2.Controls.Add(this.LogChart);
      this.LogSplitter.Size = new System.Drawing.Size(1008, 461);
      this.LogSplitter.SplitterDistance = 145;
      this.LogSplitter.TabIndex = 0;
      // 
      // LogTree
      // 
      this.LogTree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.LogTree.Location = new System.Drawing.Point(3, 3);
      this.LogTree.Name = "LogTree";
      this.LogTree.Size = new System.Drawing.Size(143, 455);
      this.LogTree.TabIndex = 0;
      this.LogTree.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.LogTree_BeforeSelect);
      this.LogTree.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.LogTree_AfterSelect);
      // 
      // LogChart
      // 
      this.LogChart.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      chartArea2.Name = "ChartArea1";
      this.LogChart.ChartAreas.Add(chartArea2);
      this.LogChart.Location = new System.Drawing.Point(3, 3);
      this.LogChart.Name = "LogChart";
      this.LogChart.Size = new System.Drawing.Size(853, 455);
      this.LogChart.TabIndex = 0;
      this.LogChart.Text = "chart1";
      this.LogChart.Click += new System.EventHandler(this.LogChart_Click);
      // 
      // LogForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(1008, 461);
      this.Controls.Add(this.LogSplitter);
      this.Name = "LogForm";
      this.Text = "Phi Logs";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.OnFormClosing);
      this.LogSplitter.Panel1.ResumeLayout(false);
      this.LogSplitter.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.LogSplitter)).EndInit();
      this.LogSplitter.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.LogChart)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.SplitContainer LogSplitter;
    private System.Windows.Forms.TreeView LogTree;
    private System.Windows.Forms.DataVisualization.Charting.Chart LogChart;
  }
}