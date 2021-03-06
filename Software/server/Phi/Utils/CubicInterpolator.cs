﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

//
// NOTE: did not move to Eve yet because there is a function that takes PhiLog_Double - maybe refactor
// to Eve.CubicInterpolator and Phi.LogInterpolator
//

namespace Phi.Utils {

  static public class CubicInterpolator {

    private static double[] x = new double[4];
    private static double[] y = new double[4];

    static public double interpolateFromLog(PhiLog_Double log, double interpAtX) {

      double interpVal;
      int logLen = log.Length;

      if (x.Length != logLen) {
        // must realloc
        x = new double[logLen];
        y = new double[logLen];
      }

      // copy data
      int idx = 0;
      foreach (PhiLog_Double.BufferItem item in log) {
        x[idx] = item.time;
        y[idx++] = item.data;
      }

      if (logLen == 4) {
        //  use data directly
        interpVal = cubic(x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3], interpAtX);

      } else {
        // use moving average
        double[] yAvg = log.getMovingAverage();
        interpVal = cubic(x[logLen-4], yAvg[logLen-4], x[logLen-3], yAvg[logLen-3], x[logLen-2], yAvg[logLen-2], x[logLen-1], yAvg[logLen-1], interpAtX);
      }

      // DEBUG
      if (double.IsNaN(interpVal)) {
        Debugger.Break();
      }

      return interpVal;
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

      double interpVal = 0;

      try {

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

        interpVal = (-y4*x1_3*x2_2*x3 + y3*x1_3*x2_2*x4 + y4*x1_3*x2*x3_2 - y3*x1_3*x2*x4_2 - y2*x1_3*x3_2*x4
                    + y2*x1_3*x3*x4_2 + y4*x1_2*x2_3*x3 - y3*x1_2*x2_3*x4 - y4*x1_2*x2*x3_3 + y3*x1_2*x2*x4_3
                    + y2*x1_2*x3_3*x4 - y2*x1_2*x3*x4_3 - y4*x1*x2_3*x3_2 + y3*x1*x2_3*x4_2 + y4*x1*x2_2*x3_3
                    - y3*x1*x2_2*x4_3 - y2*x1*x3_3*x4_2 + y2*x1*x3_2*x4_3 + y1*x2_3*x3_2*x4 - y1*x2_3*x3*x4_2
                    - y1*x2_2*x3_3*x4 + y1*x2_2*x3*x4_3 + y1*x2*x3_3*x4_2 - y1*x2*x3_2*x4_3)
                    /
                  (-x1_3*x2_2*x3 + x1_3*x2_2*x4 + x1_3*x2*x3_2 - x1_3*x2*x4_2 - x1_3*x3_2*x4 + x1_3*x3*x4_2
                  + x1_2*x2_3*x3 - x1_2*x2_3*x4 - x1_2*x2*x3_3 + x1_2*x2*x4_3 + x1_2*x3_3*x4 - x1_2*x3*x4_3
                  - x1*x2_3*x3_2 + x1*x2_3*x4_2 + x1*x2_2*x3_3 - x1*x2_2*x4_3 - x1*x3_3*x4_2 + x1*x3_2*x4_3
                  + x2_3*x3_2*x4 - x2_3*x3*x4_2 - x2_2*x3_3*x4 + x2_2*x3*x4_3 + x2*x3_3*x4_2 - x2*x3_2*x4_3);

      } catch (Exception e) {
        Debug.WriteLine("Exception {0} in cubic interpolator", e.GetType().ToString());
      }

      return interpVal;
    }

  } // class CubicInterpolator
} // namespace
