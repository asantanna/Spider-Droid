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
  /// Interaction logic for NeuronBox.xaml
  /// </summary>
  public partial class NeuronBox : UserControl {
    public NeuronBox() {
      InitializeComponent();
    }

    // value has range [-1, 1]
    public void update(double value) {

      const double SCALE = 1.5;
      double w = ActualWidth  * Math.Abs(value * SCALE);
      double h = ActualHeight * Math.Abs(value * SCALE);

      ellipse.Height = h;
      ellipse.Width = w;

      if (value > 0) {
        ellipse.Fill = Brushes.Lime;
      } else {
        ellipse.Fill = Brushes.Red;
      }
    }

  }
}
