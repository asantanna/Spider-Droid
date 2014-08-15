using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace Phi.Utils {

  public abstract class PhiLogBase {

    //
    // VARS
    //

    string name;

    // different types of logs can use different types of buffers
    protected object buffer = null;

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

    protected Series createSeries(Chart chart, string seriesName, SeriesChartType chartType) {
      Series series = new Series();
      series.ChartArea = chart.ChartAreas[0].Name;
      series.ChartType = chartType;
      series.Name = seriesName;
      // add to chart
      chart.Series.Add(series);
      return series;
    }

    protected void deleteSeries(Chart chart, Series s) {
      chart.Series.Remove(s);
    }

    public void addToLogWindow(TreeNode parentNode, string name, string text) {
      // create log view tree node
      TreeNode logTreeNode = new TreeNode(name);
      logTreeNode.Tag = this;
      logTreeNode.Text = text;
      parentNode.Nodes.Add(logTreeNode);
    }
    
    // must be implemented by derived classes
    public abstract void populateChart(Chart chart);
    public abstract void clearChart(Chart chart);
  }
}
