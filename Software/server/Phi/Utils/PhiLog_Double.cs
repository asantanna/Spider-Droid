using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.DataVisualization.Charting;

namespace Phi.Utils {

  public class PhiLog_Double : PhiLogBase {

    private SeriesChartType chartType;
    private string dataName;
    private Series chartSeries;
    private double dataMin;
    private double dataMax;


    public PhiLog_Double(int depth, 
                         string logName,
                         bool bIsCircular = true) : base(logName) {

      setChartType(SeriesChartType.Spline);
      setDataName("double");
      chartSeries = null;

      // if dataMin == dataMax, then axis range is auto
      dataMin = 0;
      dataMax = 0;

      if (bIsCircular == true) {
        setBuffer(new CircularBuffer<BufferItem>(depth));

      } else {
        throw new NotImplementedException("Non-circular buffers not supported yet.");
      }
    }

    public void setDataName(string dataName) {
      this.dataName = dataName;
    }

    public void setDataRange(double min, double max) {
      dataMin = min;
      dataMax = max;
    }

    public void setChartType(SeriesChartType chartType) {
      this.chartType = chartType;
    }

    public override void populateChart(Chart chart) {

      // create a series for the chart
      chartSeries = createSeries(chart, dataName,  chartType);

      // populate it with log contents
      // note: lock to prevent changing by comm thread

      double minT = Double.MaxValue;
      double maxT = Double.MinValue;

      lock (this) {

        // cycle thru once to determine min and max time

        foreach (BufferItem item in (buffer as CircularBuffer<BufferItem>)) {
          double t = item.time;
          minT = Math.Min(minT, t);
          maxT = Math.Max(maxT, t);
        }

        // now add to graph (time is in mS relative to largest)

        foreach (BufferItem item in (buffer as CircularBuffer<BufferItem>)) {
          // add item - convert to mS
          double msDelta = (item.time - maxT) / 1000;
          chartSeries.Points.Add(new DataPoint(msDelta, item.data));
        }
        
      } // lock

      //
      // Set chart appearance
      //

      ChartArea chartArea = chart.ChartAreas[0];

      // time axis

      Axis axis =  chartArea.AxisX;
      axis.Title = "time (mS)";
      axis.LabelStyle.Format = "F1";
      axis.Minimum = (minT - maxT) / 1000;
      axis.Maximum = 0.000001;

      // data axis

      axis =  chartArea.AxisY;
      axis.Title = dataName;
      axis.LabelStyle.Format = "F2";

      if (dataMin != dataMax) {
        // set data range
        axis.Minimum = dataMin;
        axis.Maximum = dataMax;
      }

    }

    public override void clearChart(Chart chart) {
      if (chartSeries != null) {
        deleteSeries(chart, chartSeries);
        chartSeries = null;
      }
    }

    public void Add(UInt64 time, double data) {
      // lock to prevent change during chart population
      lock (this) {
        (buffer as CircularBuffer<BufferItem>).Add(new BufferItem(time, data));
      }
    }

    //
    // Buffer Item
    //

    class BufferItem {
      public UInt64 time;
      public double data;

      // parameterless constructor for CircularBuffer<T>
      public BufferItem() : this(0,0) {}

      public BufferItem(UInt64 time, double data) {
        this.time = time;
        this.data = data;
      }

      public string ToString(string dataName) {
        return string.Format("t: %d, %s: %g", time, dataName, data);
      }
    } // class BufferItem

  } // class PhiLogTimeScalar
} // namespace
