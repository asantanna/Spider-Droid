using Phi.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.DataVisualization.Charting;

namespace Phi {

  // this is a container log joint position seek operations

  public class PhiLog_AdaptiveSeek : PhiLog_SimpleSeek {

    UInt64 targetTime;
    Series targetTimeLine;


    // actual logs used
    PhiLog_Double log_idealTrack;
    PhiLog_Double log_interpPos;

    //
    // CODE
    //

    public PhiLog_AdaptiveSeek(string name, double targetPos, UInt64 targetTime) : base(name, targetPos) {
      this.targetTime = targetTime;

      log_idealTrack = new PhiLog_Double(preAllocLength: 100, logName: "log_idealTrack", bIsCircular: false);
      log_idealTrack.setDataName("Ideal Position");
      log_idealTrack.setDataRange(0, 1);
      log_idealTrack.chart_setMarkerStyle(MarkerStyle.None);
      log_idealTrack.chart_setLineColor(System.Drawing.Color.DarkOrange);
      log_idealTrack.chart_setLineStyle(ChartDashStyle.Dash);

      log_interpPos = new PhiLog_Double(preAllocLength: 100, logName: "log_interpPos", bIsCircular: false);
      log_interpPos.setDataName("Interp Position");
      log_interpPos.setDataRange(0, 1);
      log_interpPos.chart_setMarkerStyle(MarkerStyle.None);
      log_interpPos.chart_setLineColor(System.Drawing.Color.ForestGreen);
      log_interpPos.chart_setLineStyle(ChartDashStyle.Dot);
    }

    override public void setTimeRange(double minTime, double maxTime, double originTime) {
      base.setTimeRange(minTime, maxTime, originTime);
      log_idealTrack.setTimeRange(minTime, maxTime, originTime);
      log_interpPos.setTimeRange(minTime, maxTime, originTime);
    }

    public override void Clear() {
      base.Clear();
      log_idealTrack.Clear();
      log_interpPos.Clear();
    }

    public void Add(UInt64 time, double position, double power, UInt64 predTime, double interpPos, double idealPos) {
      base.Add(time, position, power);
      log_idealTrack.Add(predTime, idealPos);
      log_interpPos.Add(predTime, interpPos);
    }

    public override void addToChart(Chart chart) {
      // add simple seek stuff
      base.addToChart(chart);
      // now populate extra adaptive seek stuff
      log_idealTrack.addToChart(chart);
      log_interpPos.addToChart(chart);
      // add target pos line
      targetTimeLine = chart_AddVerticalLine(
          chart, "Expected End", System.Drawing.Color.MediumSpringGreen, targetTime);

    }

    public override void clearChart(Chart chart) {
      base.clearChart(chart);
      log_idealTrack.clearChart(chart);
      log_interpPos.clearChart(chart);
      chart_deleteSeries(chart, targetTimeLine);
    }

    public override void disposeLog() {
      log_idealTrack.disposeLog();
      log_interpPos.disposeLog();
      base.disposeLog();
    }

  }
}
