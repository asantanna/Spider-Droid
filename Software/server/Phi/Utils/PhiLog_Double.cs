using Eve;
using Eve.Utils;
using System;
using System.Collections.Generic;
using System.Diagnostics;
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

      setChartType(SeriesChartType.Line);
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

    public override void disposeLog() {
      // dispose base!
      (this as IDisposable).Dispose();
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

    public double[] getMovingAverage(double accumRate = 0) {
      int logLen = Length;
      Debug.Assert(logLen >= 4);
      double[] movAvg = new double[logLen];
      double accum = 0;
      int idx = 0;

      if (accumRate == 0) {
        // means avg will be used for interpolation which needs 4 items
        // calculate rate such that deepest item is 1/10 of sum:
        //    depth = len - 4
        //    h^depth = 1/10
        //      h = (1/10) ^ 1/depth
        double depth = logLen - 4; 
        accumRate = 1 - Math.Pow(1.0/10, 1.0 / depth);
      }
      
      double histRate = 1 - accumRate;
      foreach (BufferItem item in this) {
        if (idx == 0) {
          accum = item.data;
        } else {
          accum = (accum * histRate) + (histRate * item.data);
        }
        // save to array
        movAvg[idx++] = accum;
      }
      return movAvg;
    }

    //
    // Chart related functions
    //

    public void setChartType(SeriesChartType chartType) {
      this.chartType = chartType;
    }

    public override void addToChart(Chart chart) {

      // add color to palette
      chart_addToPalette(chart, chart_getLineColor());

      // create a series for the chart
      chartSeries = chart_createSeries(chart, dataName,  chartType);

      // populate it with log contents
      // note: lock to prevent changing by comm thread

      lock (this) {

        if (bIsCircular) {

          // circular buffer
          CircularBuffer<BufferItem> circBuff = (buffer as CircularBuffer<BufferItem>);

          // add to chart (time is in mS relative to origin)

          foreach (BufferItem item in circBuff) {
            // add item - convert to mS
            double timeVal = (item.time - originTimeVal) / 1000;
            chartSeries.Points.Add(new DataPoint(timeVal, item.data));
          }

        } else {

          // reusable list
          ListWithReusableElements<PhiLog_Double.BufferItem> listBuff = (buffer as ListWithReusableElements<PhiLog_Double.BufferItem>);

          // add to chart (time is in mS relative to origin)

          foreach (BufferItem item in listBuff) {
            // add item - convert to mS
            double timeVal = (item.time - originTimeVal) / 1000;
            chartSeries.Points.Add(new DataPoint(timeVal, item.data));
          }
        }
       
      } // lock

      // Set chart appearance
      ChartArea chartArea = chart.ChartAreas[0];

      // time axis

      Axis axis =  chartArea.AxisX;
      axis.Title = "time (mS)";
      axis.LabelStyle.Format = "F1";
      const double SLOP = 0.025;
      double range = maxTimeVal - minTimeVal;
      double extra = SLOP * range;
      axis.Minimum = (minTimeVal - originTimeVal - extra) / 1000;
      axis.Maximum = (maxTimeVal - originTimeVal + extra) / 1000;

      // data axis

      if (bUseY2Axis == false) {
        axis =  chartArea.AxisY;
      } else {
        axis =  chartArea.AxisY2;
      }

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
        chart_deleteSeries(chart, chartSeries);
        chartSeries = null;
      }
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
        return string.Format("t: {0}, {1}: {2:F3}", time, dataName, data);
      }
    } // class BufferItem

  } // class PhiLogTimeScalar

} // namespace
