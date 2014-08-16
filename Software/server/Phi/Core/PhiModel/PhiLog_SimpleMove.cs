using Phi.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.DataVisualization.Charting;

namespace Phi {

  // this is a container log for several logs useful for keeping track of joint move performance

  public class PhiLog_SimpleMove : PhiLogBase {

    // actual logs used
    PhiLog_Double positionLog;
    PhiLog_Double powerLog;

    //
    // CODE
    //

    public PhiLog_SimpleMove(string name) : base(name) {
      positionLog = new PhiLog_Double(preAllocLength: 100, logName: "positionLog", bIsCircular: false);
      positionLog.setDataName("position");
      positionLog.setDataRange(0, 1);
      positionLog.setMarkerColor(System.Drawing.Color.Blue);
      positionLog.setMarkerStyle(MarkerStyle.None);

      powerLog = new PhiLog_Double(preAllocLength: 100, logName: "powerLog", bIsCircular: false);
      powerLog.setDataName("power");
      powerLog.setDataRange(-1, 1);
      powerLog.setMarkerColor(System.Drawing.Color.Blue);
      powerLog.setMarkerStyle(MarkerStyle.None);
    }

    public void Clear() {
      positionLog.Clear();
      powerLog.Clear();
    }

    public void Add(UInt64 time, double position, double power) {
      positionLog.Add(time, position);
      powerLog.Add(time, power);
    }

    public override void populateChart(Chart chart) {
      positionLog.populateChart(chart);
      powerLog.populateChart(chart);
    }

    public override void clearChart(Chart chart) {
      positionLog.clearChart(chart);
      powerLog.clearChart(chart);
    }

  }
}
