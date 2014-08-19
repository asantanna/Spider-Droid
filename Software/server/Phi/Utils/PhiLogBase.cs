using Phi.UI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace Phi.Utils {

  public abstract class PhiLogBase : IDisposable {

    //
    // VARS
    //

    public readonly string name;

    // different types of logs can use different types of buffers
    protected object buffer = null;

    // for window and chart control
    private TreeNode logTreeNode;
    private MarkerStyle markerStyle = MarkerStyle.Circle;
    private System.Drawing.Color markerColor = System.Drawing.Color.Red;
    private System.Drawing.Color lineColor = System.Drawing.Color.Blue;
    protected bool bUseY2Axis = false;
    private int lineWidth = 2;
    private ChartDashStyle lineStyle = ChartDashStyle.Solid;

    protected double minTimeVal;
    protected double originTimeVal;
    protected double maxTimeVal;

    //
    // CODE
    //

    public PhiLogBase (string name) {
      this.name = name;
      // buffer not passed in constructor because it causes
      // problems in derived classes since base constructors 
      // need to be called at the declaration line
    }

    protected void setBuffer(object bufferToUse) {
      this.buffer = bufferToUse;
    }

    public void chart_setMarkerColor(System.Drawing.Color color) {
      markerColor = color;
    }

    public void chart_setMarkerStyle(MarkerStyle style) {
      markerStyle = style;
    }

    public System.Drawing.Color chart_getLineColor() {
      return lineColor;
    }

    public void chart_setLineColor(System.Drawing.Color color) {
      lineColor = color;
    }


    public void chart_useY2Axis() {
      bUseY2Axis = true;
    }

    public void chart_setLineWidth(int width) {
      lineWidth = width;
    }

    public void chart_setLineStyle(ChartDashStyle style) {
      lineStyle = style;
    }

    virtual public void setTimeRange(double minTime, double maxTime, double originTime) {
      this.minTimeVal = minTime;
      this.maxTimeVal = maxTime;
      this.originTimeVal = originTime;
    }

    protected Series chart_createSeries(Chart chart, string dataName, SeriesChartType chartType) {
      Series series = new Series();
      ChartArea chartArea = chart.ChartAreas[0];

      // set basic info
      series.Name = dataName;
      series.ChartArea = chartArea.Name;
      series.ChartType = chartType;

      // set trace type
      series.BorderWidth = lineWidth;
      series.BorderDashStyle = lineStyle;

      // set marker type
      series.MarkerStyle = markerStyle;
      series.MarkerColor = markerColor;

      // set Y axis to use (left or right)
      if (bUseY2Axis) {
        series.YAxisType = AxisType.Secondary;
        chartArea.AxisY2.Enabled = AxisEnabled.True;
      }

      // add legend
      Legend legend = new Legend(dataName);
      series.Legend = dataName;
      series.IsVisibleInLegend = true;
      legend.DockedToChartArea = chartArea.Name;
      chart.Legends.Add(legend);

      // add to chart
      chart.Series.Add(series);
      return series;
    }

    protected void chart_deleteSeries(Chart chart, Series s) {
      chart.Series.Remove(s);
      chart.Legends.Clear();
    }

    private Series chart_addLine(Chart chart, string dataName, System.Drawing.Color color, double value, bool bVertical) {
      Series series = new Series();
      ChartArea chartArea = chart.ChartAreas[0];

      series.Name = dataName;
      series.ChartArea = chartArea.Name;
      series.ChartType = SeriesChartType.Line;
      series.BorderWidth = 2;
      series.BorderDashStyle = ChartDashStyle.Dash;
      series.MarkerStyle = MarkerStyle.None;

      if (bVertical) {
        series.Points.Add(new DataPoint(value, chartArea.AxisY.Minimum));
        series.Points.Add(new DataPoint(value, chartArea.AxisY.Maximum));
      } else {
        series.Points.Add(new DataPoint(chartArea.AxisX.Minimum, value));
        series.Points.Add(new DataPoint(chartArea.AxisX.Maximum, value));
      }

      // add color to palette
      chart_addToPalette(chart, color);

      // add to chart
      chart.Series.Add(series);
      return series;
    }

    protected Series chart_AddVerticalLine(Chart chart, string dataName, System.Drawing.Color color, double xValue) {
      // x is time - adjust to origin and convert to mS
      xValue = (xValue - originTimeVal) / 1000;
      return chart_addLine(chart, dataName, color, xValue, true);
    }

    protected Series chart_AddHorizontalLine(Chart chart, string dataName, System.Drawing.Color color, double yValue) {
      return chart_addLine(chart, dataName, color, yValue, false);
    }

    protected void chart_clearPalette(Chart chart) {
      chart.Palette = ChartColorPalette.None;
    }

    protected void chart_addToPalette(Chart chart, params System.Drawing.Color[] colors) {
      // add some colors to palette
      List<System.Drawing.Color> newColors = new List<System.Drawing.Color>(chart.PaletteCustomColors);
      newColors.AddRange(colors);
      chart.PaletteCustomColors = newColors.ToArray();
    }

    public void addToLogWindow(TreeNode parentNode, string name, string text) {
      // create log view tree node
      logTreeNode = new TreeNode(name);
      logTreeNode.Tag = this;
      logTreeNode.Text = text;
      LogForm.invokeAddNode(logTreeNode, parentNode);
    }

    public void removeLogFromWindow() {
      if (logTreeNode != null) {
        LogForm.invokeRemove(logTreeNode);
        logTreeNode = null;
      }
    }

    void IDisposable.Dispose() {
      removeLogFromWindow();
    }

    public void populateChart(Chart chart) {
      // clear palette
      chart_clearPalette(chart);
      // add
      addToChart(chart);
    }
  
    //
    // MUST BE IMPLEMENTED BY DERIVED CLASSES
    //

    public abstract void addToChart(Chart chart);
    public abstract void clearChart(Chart chart);
    public abstract void disposeLog();

  } // class PhiLogBase
} // namespace
