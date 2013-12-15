using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi.Utils {
  static class InfoT {

    public double entropy(PDF pdf) {
      double H = 0;
      for (int i = 0; i < pdf.getNumElem(); i++) {
        double elem = pdf.getPDF(i);
        H += elem * Math.Log(elem, 2);
      }
      return -H;
    }

    public double entropy(PDF pdf1, PDF pdf2) {
      double H = 0;
      return -H;
    }

    public double mutualInfo(PDF pdf1, PDF pdf2) {
      return entropy(pdf1) + entropy(pdf2) - entropy(pdf1, pdf2);
    }

    public void test() {
      PDF pdf = new PDF(1, 2);
      for (double p = 0; p <= 1.0; p += 0.1) {
        pdf.setPDF(new double[2] { p, 1 - p });
        Console.WriteLine("H" + p + "," + (1 - p) + "=" + entropy(pdf));
      } 
    }
  }
}
