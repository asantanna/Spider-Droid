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
  /// Interaction logic for LegBox.xaml
  /// </summary>
  public partial class LegBox : UserControl {
    public LegBox() {
      InitializeComponent();
    }

    internal void update(
      double hipNeuronVal, double hipJointPos,
      double thighNeuronVal, double thighJointPos,
      double kneeNeuronVal, double kneeJointPos) {

      hipJoint.update(hipNeuronVal, hipJointPos);
      thighJoint.update(thighNeuronVal, thighJointPos);
      kneeJoint.update(kneeNeuronVal, kneeJointPos);
    }

  }
}
