using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi.Utils {
  public static class InfoT {

    public static double entropy(PDF pdf) {
      double H = 0;
      for (int i = 0; i < pdf.getNumElem(); i++) {
        double elem = pdf.getPDF(i);
        H += elem * Math.Log(elem, 2);
      }
      return -H;
    }

    public static double entropy(PDF pdf1, PDF pdf2) {
      double H = 0;
      return -H;
    }

    public static double mutualInfo(PDF pdf1, PDF pdf2) {
      return entropy(pdf1) + entropy(pdf2) - entropy(pdf1, pdf2);
    }

    public static void test() {
      PDF pdf = new PDF(1, 2);

      Console.WriteLine("Entropy:");
      for(double p = 0; p <= 1.0; p += 0.1) {
        pdf.setPDF(new double[2] { p, 1 - p });
        Console.WriteLine("H({0:F}, {1:F}) = {2:F}", p, (1-p), entropy(pdf));
      } 
    }
  }
}
