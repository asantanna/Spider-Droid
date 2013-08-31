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

namespace Phi_Core {
  /// <summary>
  /// Interaction logic for JointBox.xaml
  /// </summary>
  public partial class JointBox : UserControl {
    public JointBox() {
      InitializeComponent();
    }

    public void update(double neuronVal, double jointPos) {
      neuronBox.update(neuronVal);
      barBox.update(jointPos);
    }

  }
}
