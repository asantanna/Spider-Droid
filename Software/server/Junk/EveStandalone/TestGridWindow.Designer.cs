namespace Eve {
  partial class TestGridWindow {
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
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.TestGridPanel = new System.Windows.Forms.FlowLayoutPanel();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.tbGridLevels = new System.Windows.Forms.TextBox();
      this.label4 = new System.Windows.Forms.Label();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.cbShowFields = new System.Windows.Forms.CheckBox();
      this.cbShowPixels = new System.Windows.Forms.CheckBox();
      this.cbShowGrid = new System.Windows.Forms.CheckBox();
      this.label3 = new System.Windows.Forms.Label();
      this.label5 = new System.Windows.Forms.Label();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.cbField0 = new System.Windows.Forms.CheckBox();
      this.cbField2 = new System.Windows.Forms.CheckBox();
      this.GridWindow = new System.Windows.Forms.PictureBox();
      this.tbStatus = new System.Windows.Forms.TextBox();
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.Panel2.SuspendLayout();
      this.splitContainer1.SuspendLayout();
      this.TestGridPanel.SuspendLayout();
      this.groupBox1.SuspendLayout();
      this.groupBox2.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.GridWindow)).BeginInit();
      this.SuspendLayout();
      // 
      // splitContainer1
      // 
      this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer1.Location = new System.Drawing.Point(0, 0);
      this.splitContainer1.Name = "splitContainer1";
      // 
      // splitContainer1.Panel1
      // 
      this.splitContainer1.Panel1.Controls.Add(this.TestGridPanel);
      // 
      // splitContainer1.Panel2
      // 
      this.splitContainer1.Panel2.Controls.Add(this.GridWindow);
      this.splitContainer1.Size = new System.Drawing.Size(670, 514);
      this.splitContainer1.SplitterDistance = 152;
      this.splitContainer1.TabIndex = 0;
      // 
      // TestGridPanel
      // 
      this.TestGridPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.TestGridPanel.Controls.Add(this.label2);
      this.TestGridPanel.Controls.Add(this.label1);
      this.TestGridPanel.Controls.Add(this.tbGridLevels);
      this.TestGridPanel.Controls.Add(this.label4);
      this.TestGridPanel.Controls.Add(this.groupBox1);
      this.TestGridPanel.Controls.Add(this.label3);
      this.TestGridPanel.Controls.Add(this.label5);
      this.TestGridPanel.Controls.Add(this.groupBox2);
      this.TestGridPanel.Controls.Add(this.tbStatus);
      this.TestGridPanel.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
      this.TestGridPanel.Location = new System.Drawing.Point(0, 0);
      this.TestGridPanel.Name = "TestGridPanel";
      this.TestGridPanel.Size = new System.Drawing.Size(149, 514);
      this.TestGridPanel.TabIndex = 0;
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(3, 0);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(0, 13);
      this.label2.TabIndex = 6;
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(3, 13);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(60, 13);
      this.label1.TabIndex = 4;
      this.label1.Text = "Grid Levels";
      // 
      // tbGridLevels
      // 
      this.tbGridLevels.Location = new System.Drawing.Point(3, 29);
      this.tbGridLevels.Name = "tbGridLevels";
      this.tbGridLevels.Size = new System.Drawing.Size(100, 20);
      this.tbGridLevels.TabIndex = 5;
      this.tbGridLevels.Text = "5";
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(3, 52);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(0, 13);
      this.label4.TabIndex = 9;
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.cbShowFields);
      this.groupBox1.Controls.Add(this.cbShowPixels);
      this.groupBox1.Controls.Add(this.cbShowGrid);
      this.groupBox1.Location = new System.Drawing.Point(3, 68);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(133, 102);
      this.groupBox1.TabIndex = 7;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Options";
      // 
      // cbShowFields
      // 
      this.cbShowFields.AutoSize = true;
      this.cbShowFields.Checked = true;
      this.cbShowFields.CheckState = System.Windows.Forms.CheckState.Checked;
      this.cbShowFields.Location = new System.Drawing.Point(10, 68);
      this.cbShowFields.Name = "cbShowFields";
      this.cbShowFields.Size = new System.Drawing.Size(83, 17);
      this.cbShowFields.TabIndex = 2;
      this.cbShowFields.Text = "Show Fields";
      this.cbShowFields.UseVisualStyleBackColor = true;
      // 
      // cbShowPixels
      // 
      this.cbShowPixels.AutoSize = true;
      this.cbShowPixels.Checked = true;
      this.cbShowPixels.CheckState = System.Windows.Forms.CheckState.Checked;
      this.cbShowPixels.Location = new System.Drawing.Point(10, 44);
      this.cbShowPixels.Name = "cbShowPixels";
      this.cbShowPixels.Size = new System.Drawing.Size(83, 17);
      this.cbShowPixels.TabIndex = 1;
      this.cbShowPixels.Text = "Show Pixels";
      this.cbShowPixels.UseVisualStyleBackColor = true;
      // 
      // cbShowGrid
      // 
      this.cbShowGrid.AutoSize = true;
      this.cbShowGrid.Checked = true;
      this.cbShowGrid.CheckState = System.Windows.Forms.CheckState.Checked;
      this.cbShowGrid.Location = new System.Drawing.Point(10, 20);
      this.cbShowGrid.Name = "cbShowGrid";
      this.cbShowGrid.Size = new System.Drawing.Size(75, 17);
      this.cbShowGrid.TabIndex = 0;
      this.cbShowGrid.Text = "Show Grid";
      this.cbShowGrid.UseVisualStyleBackColor = true;
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(3, 173);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(0, 13);
      this.label3.TabIndex = 8;
      // 
      // label5
      // 
      this.label5.AutoSize = true;
      this.label5.Location = new System.Drawing.Point(3, 186);
      this.label5.Name = "label5";
      this.label5.Size = new System.Drawing.Size(0, 13);
      this.label5.TabIndex = 11;
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.cbField0);
      this.groupBox2.Controls.Add(this.cbField2);
      this.groupBox2.Location = new System.Drawing.Point(3, 202);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(113, 75);
      this.groupBox2.TabIndex = 10;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Enabled Fields";
      // 
      // cbField0
      // 
      this.cbField0.AutoSize = true;
      this.cbField0.Checked = true;
      this.cbField0.CheckState = System.Windows.Forms.CheckState.Checked;
      this.cbField0.Location = new System.Drawing.Point(9, 19);
      this.cbField0.Name = "cbField0";
      this.cbField0.Size = new System.Drawing.Size(61, 17);
      this.cbField0.TabIndex = 0;
      this.cbField0.Text = "Level 0";
      this.cbField0.UseVisualStyleBackColor = true;
      // 
      // cbField2
      // 
      this.cbField2.AutoSize = true;
      this.cbField2.Checked = true;
      this.cbField2.CheckState = System.Windows.Forms.CheckState.Checked;
      this.cbField2.Location = new System.Drawing.Point(9, 42);
      this.cbField2.Name = "cbField2";
      this.cbField2.Size = new System.Drawing.Size(61, 17);
      this.cbField2.TabIndex = 1;
      this.cbField2.Text = "Level 1";
      this.cbField2.UseVisualStyleBackColor = true;
      // 
      // GridWindow
      // 
      this.GridWindow.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(40)))), ((int)(((byte)(40)))));
      this.GridWindow.Cursor = System.Windows.Forms.Cursors.Cross;
      this.GridWindow.Location = new System.Drawing.Point(11, 7);
      this.GridWindow.Name = "GridWindow";
      this.GridWindow.Size = new System.Drawing.Size(500, 500);
      this.GridWindow.TabIndex = 0;
      this.GridWindow.TabStop = false;
      this.GridWindow.Paint += new System.Windows.Forms.PaintEventHandler(this.GridWindow_Paint);
      this.GridWindow.MouseDown += new System.Windows.Forms.MouseEventHandler(this.GridWindow_MouseDown);
      this.GridWindow.MouseMove += new System.Windows.Forms.MouseEventHandler(this.GridWindow_MouseMove);
      this.GridWindow.MouseUp += new System.Windows.Forms.MouseEventHandler(this.GridWindow_MouseUp);
      // 
      // tbStatus
      // 
      this.tbStatus.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.tbStatus.BackColor = System.Drawing.SystemColors.MenuBar;
      this.tbStatus.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.tbStatus.Location = new System.Drawing.Point(3, 283);
      this.tbStatus.Multiline = true;
      this.tbStatus.Name = "tbStatus";
      this.tbStatus.ReadOnly = true;
      this.tbStatus.Size = new System.Drawing.Size(133, 228);
      this.tbStatus.TabIndex = 12;
      // 
      // TestGridWindow
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(670, 514);
      this.Controls.Add(this.splitContainer1);
      this.Name = "TestGridWindow";
      this.Text = "Grid Field Test UI";
      this.splitContainer1.Panel1.ResumeLayout(false);
      this.splitContainer1.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
      this.splitContainer1.ResumeLayout(false);
      this.TestGridPanel.ResumeLayout(false);
      this.TestGridPanel.PerformLayout();
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      this.groupBox2.ResumeLayout(false);
      this.groupBox2.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.GridWindow)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.SplitContainer splitContainer1;
    private System.Windows.Forms.FlowLayoutPanel TestGridPanel;
    private System.Windows.Forms.CheckBox cbField0;
    private System.Windows.Forms.CheckBox cbField2;
    private System.Windows.Forms.PictureBox GridWindow;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.TextBox tbGridLevels;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.CheckBox cbShowFields;
    private System.Windows.Forms.CheckBox cbShowPixels;
    private System.Windows.Forms.CheckBox cbShowGrid;
    private System.Windows.Forms.TextBox tbStatus;
  }
}