using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Eve.Utils {
  public static class InfoT {

    public static double entropy(PDF pdf) {
      double H = 0;
      for (int i = 0; i < pdf.getNumElem(); i++) {
        double prob = pdf.getProb(i);
        if(prob > 0.0001)
          H += prob * Math.Log(prob, 2);
      }
      return -H;
    }

    public static double mutualInfo(PDF pdf1, PDF pdf2, PDF pdfJoint) {
      return entropy(pdf1) + entropy(pdf2) - entropy(pdfJoint);
    }

    public static void test() {
      PDF pdf = new PDF(1, 2);

      Console.WriteLine("Entropy:");
      for(double p = 0; p <= 1.0; p += 0.1) {
        pdf.setPDF(new double[2] { p, 1 - p });
        Console.WriteLine("H({0:F}, {1:F}) = {2:F}", p, (1-p), entropy(pdf));
      }

      //create fair coin
      pdf.setPDF(new double[2] {0.5, 0.5});

      //create two fair coins
      PDF pdfJoint = new PDF(2, 2);
      pdfJoint.setPDF(new double[4] { 0.25, 0.25, 0.25, 0.25 });

      Console.WriteLine("\nTest Mutual Information:\nThe mutual information between two fair coins is {0:F}", mutualInfo(pdf, pdf, pdfJoint));

      pdfJoint.setPDF(new double[4] { 0.0, 0.5, 0.5, 0.0 });

      Console.WriteLine("The mutual information between two entangled quantum coins is {0:F}", mutualInfo(pdf, pdf, pdfJoint));

      pdfJoint.setPDF(new double[4] { 0.1, 0.4, 0.3, 0.2 });

      Console.WriteLine("The mutual information between two rigged coins is {0:F}", mutualInfo(pdf, pdf, pdfJoint));

    }
  }
}
