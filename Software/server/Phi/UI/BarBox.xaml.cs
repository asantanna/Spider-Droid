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
  /// Interaction logic for BarBox.xaml
  /// </summary>
  public partial class BarBox : UserControl {
    public BarBox() {
      InitializeComponent();
    }

    // value has range [0, 1]
    public void update(double value) {

      double h = ActualHeight * value;
      bar.Height = h;

      if ((value < 0.1) || (value > 0.9)) {
        bar.Fill = Brushes.Red;
      } else {
        bar.Fill = Brushes.Lime;
      }
    }

  }
}
