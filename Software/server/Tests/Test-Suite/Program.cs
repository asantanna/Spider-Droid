using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Test_Suite {
  class Program {

    static Random rand = new Random(unchecked(DateTime.Now.Ticks.GetHashCode()));

    static int getRand() {
        return rand.Next(0, int.MaxValue);
    }

    static void Main(string[] args) {

      // one coin

      PDF pdf = new PDF(1,2);

      int[] a1 = new int[1];
      int[] a2 = new int[2];
      int numtimes = 10000;

      for(int i=0 ; i<numtimes ; i++)  {
        int c = getRand() % 2;
        a1[0] = c;
        pdf.updatePDF(a1);
      }

      int[] h = new int[1] { 0 };
      int[] t = new int[1] { 1 };

      Console.WriteLine("p(heads) = " + pdf.getPDF(h) + ", p(tails) = " + pdf.getPDF(t));
  
      // two coins

      pdf = new PDF(2, 2);
      for(int i=0 ; i<numtimes ; i++)  {
        int c1 = getRand() % 2;
        a2[0] = c1;
        int c2 = getRand() % 2;
        a2[1] = c2;
        pdf.updatePDF(a2);
      }
  
      int[] i1 = new int[2] {0,0};
      int[] i2 = new int[2] {0,1};
      int[] i3 = new int[2] {1,0};
      int[] i4 = new int[2] {1,1};

      Console.WriteLine("p(heads,heads) = " + pdf.getPDF(i1) + "\np(heads,tails) = " + pdf.getPDF(i2) + 
        "\np(tails,heads) = " + pdf.getPDF(i3) + "\np(tails,tails) = " + pdf.getPDF(i4));

      // test offset to index conversion

      Console.WriteLine("Testing off -> idx");

      pdf.getIdxs(0, i1);
      Console.WriteLine("i1[0] = " + i1[0] + ", i1[1] = " + i1[1]);

      pdf.getIdxs(1, i1);
      Console.WriteLine("i1[0] = " + i1[0] + ", i1[1] = " + i1[1]);

      pdf.getIdxs(2, i1);
      Console.WriteLine("i1[0] = " + i1[0] + ", i1[1] = " + i1[1]);

      pdf.getIdxs(3, i1);
      Console.WriteLine("i1[0] = " + i1[0] + ", i1[1] = " + i1[1]);
 
      // one coin sampling

      pdf = new PDF(1, 2);
      double[] prob = new double[2] {0.7f, 0.3f};
      pdf.setPDF(prob);

      int[] idxs = new int[1];
      int[] counts = new int[2] {0,0};

      for(int i=0 ; i<numtimes ; i++)  {
        pdf.samplePDF(idxs);
        counts[idxs[0]]++;
      }

      Console.WriteLine("numHeads = " + counts[0] + ", numTails = " + counts[1]);

      Console.ReadLine();
    }
  }
}
