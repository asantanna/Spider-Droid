using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi.Utils {

  static public class CubicInterpolator {

    private static double[] x = new double[4];
    private static double[] y = new double[4];

    static public double interpolateFromLog(PhiLog_Double log, double interpAtX) {

      if (log.Length != 4) {
        throw new InvalidProgramException("interpolateFromLog requires a log that is exactly 4 items long");
      }
      
      int idx = 0;
      foreach (PhiLog_Double.BufferItem item in log) {
        x[idx] = item.time;
        y[idx] = item.data;
        idx++;
      }

      return cubic(x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3], interpAtX);
    }

    // Cubic interpolation function

    static public double cubic(
      double x1,
      double y1,
      double x2,
      double y2,
      double x3,
      double y3,
      double x4,
      double y4,
      double x5) {

      //shift all x coordinates so x5 equals zero
      x1 -= x5;
      x2 -= x5;
      x3 -= x5;
      x4 -= x5;

      //compute squares and cubes of each x coordinate
      double x1_2 = Math.Pow(x1, 2);
      double x1_3 = Math.Pow(x1, 3);
      double x2_2 = Math.Pow(x2, 2);
      double x2_3 = Math.Pow(x2, 3);
      double x3_2 = Math.Pow(x3, 2);
      double x3_3 = Math.Pow(x3, 3);
      double x4_2 = Math.Pow(x4, 2);
      double x4_3 = Math.Pow(x4, 3);

      //compute y5 and return it

      return (-y4*x1_3*x2_2*x3 + y3*x1_3*x2_2*x4 + y4*x1_3*x2*x3_2 - y3*x1_3*x2*x4_2 - y2*x1_3*x3_2*x4
        + y2*x1_3*x3*x4_2 + y4*x1_2*x2_3*x3 - y3*x1_2*x2_3*x4 - y4*x1_2*x2*x3_3 + y3*x1_2*x2*x4_3
        + y2*x1_2*x3_3*x4 - y2*x1_2*x3*x4_3 - y4*x1*x2_3*x3_2 + y3*x1*x2_3*x4_2 + y4*x1*x2_2*x3_3
        - y3*x1*x2_2*x4_3 - y2*x1*x3_3*x4_2 + y2*x1*x3_2*x4_3 + y1*x2_3*x3_2*x4 - y1*x2_3*x3*x4_2
        - y1*x2_2*x3_3*x4 + y1*x2_2*x3*x4_3 + y1*x2*x3_3*x4_2 - y1*x2*x3_2*x4_3)
       /
       (-x1_3*x2_2*x3 + x1_3*x2_2*x4 + x1_3*x2*x3_2 - x1_3*x2*x4_2 - x1_3*x3_2*x4 + x1_3*x3*x4_2
        + x1_2*x2_3*x3 - x1_2*x2_3*x4 - x1_2*x2*x3_3 + x1_2*x2*x4_3 + x1_2*x3_3*x4 - x1_2*x3*x4_3
        - x1*x2_3*x3_2 + x1*x2_3*x4_2 + x1*x2_2*x3_3 - x1*x2_2*x4_3 - x1*x3_3*x4_2 + x1*x3_2*x4_3
        + x2_3*x3_2*x4 - x2_3*x3*x4_2 - x2_2*x3_3*x4 + x2_2*x3*x4_3 + x2*x3_3*x4_2 - x2*x3_2*x4_3);
    }

  } // class CubicInterpolator
} // namespace
