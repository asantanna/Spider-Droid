using Phi.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.DataVisualization.Charting;

namespace Phi {

  // this is a container log for for keeping track of simple (non-adaptive) joint move performance

  public class PhiLog_SimpleMove : PhiLogBase {

    // actual logs used
    PhiLog_Double log_position;
    PhiLog_Double log_power;
    Series completeLine;

    //
    // CODE
    //

    public PhiLog_SimpleMove(string name) : base(name) {
      log_position = new PhiLog_Double(preAllocLength: 100, logName: "log_position", bIsCircular: false);
      log_position.setDataName("Position");
      log_position.setDataRange(0, 1);
      log_position.chart_setMarkerStyle(MarkerStyle.None);
      log_position.chart_setLineColor(System.Drawing.Color.Blue);

      log_power = new PhiLog_Double(preAllocLength: 100, logName: "log_power", bIsCircular: false);
      log_power.setDataName("Power");
      log_power.setDataRange(-1, 1);
      log_power.chart_setMarkerStyle(MarkerStyle.None);
      log_power.chart_setLineColor(System.Drawing.Color.Red);
      log_power.chart_useY2Axis();
    }

    override public void setTimeRange(double minTime, double maxTime, double originTime) {
      base.setTimeRange(minTime, maxTime, originTime);
      log_position.setTimeRange(minTime, maxTime, originTime);
      log_power.setTimeRange(minTime, maxTime, originTime);
    }

    public virtual void Clear() {
      log_position.Clear();
      log_power.Clear();
    }

    public void Add(UInt64 time, double position, double power) {
      log_position.Add(time, position);
      log_power.Add(time, power);
    }

    public override void addToChart(Chart chart) {
      // now add internal logs
      log_position.addToChart(chart);
      log_power.addToChart(chart);

      // add command complete line
      completeLine = chart_AddVerticalLine(chart, "Completion Time", System.Drawing.Color.DarkViolet, originTimeVal);
    }

    public override void clearChart(Chart chart) {
      log_position.clearChart(chart);
      log_power.clearChart(chart);
      chart_deleteSeries(chart, completeLine);
    }

    public override void disposeLog() {
      log_position.disposeLog();
      log_power.disposeLog();
      // dispose base!
      // Note: Base is PhiLogBase so call Dispose() instead of disposeLog()
      (this as IDisposable).Dispose();
    }

  } // class PhiLog_SimpleMove
} // namespace
