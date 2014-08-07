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
using System.Windows.Shapes;

// Dialog for selecting a leg

namespace Phi.UI {
  public partial class DlgSelectLeg : Window {

    int selectedLegIdx = 0;

    public DlgSelectLeg() {
      InitializeComponent();
    }

    private void btnOK_click(object sender, RoutedEventArgs e) {
      selectedLegIdx = comboSelectLeg.SelectedIndex;
      this.DialogResult = true;
    }

    private void btnCancel_click(object sender, RoutedEventArgs e) {
      this.DialogResult = false;
    }

    public int getLegIdx() {
      return selectedLegIdx;
    }
  }
}
