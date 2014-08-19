using Phi.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.DataVisualization.Charting;

namespace Phi {

  // this is a container log for joint seek operations

  public class PhiLog_SimpleSeek : PhiLog_SimpleMove {

    double targetPos;
    Series targetPosLine;

    //
    // CODE
    //

    public PhiLog_SimpleSeek(string name, double targetPos) : base(name) {
      this.targetPos = targetPos;
    }

    public override void addToChart(Chart chart) {
      // add position and power
      base.addToChart(chart);
      // add target pos line
      targetPosLine = chart_AddHorizontalLine(chart, "Seek Target", System.Drawing.Color.DeepPink, targetPos);
    }

    public override void clearChart(Chart chart) {
      base.clearChart(chart);
      chart_deleteSeries(chart, targetPosLine);
    }      

  } // class PhiLog_SimpleSeek
} // namespace
