using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Phi {

  /// <summary>
  /// Interaction logic for PieControl.xaml
  /// </summary>
  /// 
  public partial class PieControl : UserControl {

    public PieControl() {
      InitializeComponent();
    }

    public void update(double degrees) {

      double radius = canvasName.ActualHeight / 2;

      // note: degrees is [-180,1]
      double endAngle = Math.PI * degrees / 180;

      double xComp = radius * Math.Cos(endAngle);
      double yComp = radius * Math.Sin(endAngle);
      if (yComp >= 0) {
        arcName.SweepDirection = SweepDirection.Clockwise;
      } else {
        arcName.SweepDirection = SweepDirection.Counterclockwise;
      }

      arcName.Point = new Point(radius + yComp, radius - xComp);

      valueName.Content = degrees.ToString("F1") + " \u00B0";
    }
  }
}
