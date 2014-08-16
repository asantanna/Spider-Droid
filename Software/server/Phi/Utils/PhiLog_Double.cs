using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.DataVisualization.Charting;

namespace Phi.Utils {

  public class PhiLog_Double : PhiLogBase, IEnumerable<PhiLog_Double.BufferItem> {

    private SeriesChartType chartType;
    private string dataName;
    private bool bIsCircular;
    private Series chartSeries;
    private double dataMin;
    private double dataMax;

    public int Length { 
      get {
        if (bIsCircular) {
          return (buffer as CircularBuffer<PhiLog_Double.BufferItem>).Length;
        } else {
          return (buffer as ListWithReusableElements<PhiLog_Double.BufferItem>).Count;
        }
      }
    }

    //
    // Code
    //

    public PhiLog_Double(int preAllocLength, 
                         string logName,
                         bool bIsCircular = true) : base(logName) {
      
      this.bIsCircular = bIsCircular;

      setChartType(SeriesChartType.Spline);
      setDataName("double");
      chartSeries = null;

      // if dataMin == dataMax, then axis range is auto
      dataMin = 0;
      dataMax = 0;

      if (bIsCircular == true) {
        setBuffer(new CircularBuffer<BufferItem>(preAllocLength));
      } else {
        setBuffer(new ListWithReusableElements<BufferItem>(preAllocLength));
      }
    }

    public void setDataName(string dataName) {
      this.dataName = dataName;
    }

    public void setDataRange(double min, double max) {
      dataMin = min;
      dataMax = max;
    }

    public void Clear() {
      // only makes sense with non-circular buffers
      if (!bIsCircular) {
        (buffer as ListWithReusableElements<BufferItem>).Clear();
      }
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

        if (bIsCircular) {

          // circular buffer
          CircularBuffer<BufferItem> circBuff = (buffer as CircularBuffer<BufferItem>);

          // cycle thru once to determine min and max time

          foreach (BufferItem item in circBuff) {
            double t = item.time;
            minT = Math.Min(minT, t);
            maxT = Math.Max(maxT, t);
          }

          // now add to graph (time is in mS relative to largest)

          foreach (BufferItem item in circBuff) {
            // add item - convert to mS
            double msDelta = (item.time - maxT) / 1000;
            chartSeries.Points.Add(new DataPoint(msDelta, item.data));
          }

        } else {

          // reusable list
          ListWithReusableElements<PhiLog_Double.BufferItem> listBuff = (buffer as ListWithReusableElements<PhiLog_Double.BufferItem>);

          // cycle thru once to determine min and max time

          foreach (BufferItem item in listBuff) {
            double t = item.time;
            minT = Math.Min(minT, t);
            maxT = Math.Max(maxT, t);
          }

          // now add to graph (time is in mS relative to largest)

          foreach (BufferItem item in listBuff) {
            // add item - convert to mS
            double msDelta = (item.time - maxT) / 1000;
            chartSeries.Points.Add(new DataPoint(msDelta, item.data));
          }
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
      double range = (maxT - minT) / 1000;
      const double SLOP = 0.1;
      axis.Minimum = -range * (1 + SLOP);
      axis.Maximum = range * SLOP;

      // data axis

      axis =  chartArea.AxisY;
      axis.Title = dataName;
      axis.LabelStyle.Format = "F1";

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
      BufferItem newItem = new BufferItem(time, data);
      // lock to prevent change during chart population
      lock (this) {
        if (bIsCircular) {
          // circular buffer
          (buffer as CircularBuffer<BufferItem>).Add(newItem);
        } else {
          // reusable list
          ListWithReusableElements<PhiLog_Double.BufferItem> list = (buffer as ListWithReusableElements<PhiLog_Double.BufferItem>);
          BufferItem item = list.getNewItem();
          item.time = time;
          item.data = data;
        } 
      } // lock

    }

    //
    // IEnumerator interface
    //

    IEnumerator<PhiLog_Double.BufferItem> IEnumerable<PhiLog_Double.BufferItem>.GetEnumerator() {
      return (buffer as IEnumerable<PhiLog_Double.BufferItem>).GetEnumerator();
    }

    System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() {
      // pass on to specific one
      return (this as IEnumerable<PhiLog_Double.BufferItem>).GetEnumerator();
    }

    //
    // Buffer Item
    //

    public class BufferItem {
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
