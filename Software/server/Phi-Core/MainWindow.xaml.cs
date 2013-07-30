using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace Phi_Core {
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {

    //
    // VARS
    //

    private static DispatcherTimer blinkyTimer;
    private bool blinkState = false;

    //
    // CODE
    //

    public MainWindow() {

      InitializeComponent();

      //
      // One-time init
      //

      // save ref
      PhiGlobals.mainWindow = this;
    }

    async private void ActivateLink_Click(object sender, RoutedEventArgs e) {

      // disable button
      ActivateLink.Content = "ACTIVE";
      ActivateLink.IsEnabled = false;

      // start listener (ASYNC)
      Task<NetworkStream> listenTask =  PhiLink.startListenerAsync();

      // wait for connection (await causes immediate suspend)
      NetworkStream phiStream = await listenTask;

      // CONNECTED - start comm loop
      if (phiStream != null) {
        PhiLink.startCommLoop(phiStream);
      }
    }

    private void enableBlinkyTimer(bool bEnable) {

      if (blinkyTimer == null) {
        blinkyTimer = new DispatcherTimer(DispatcherPriority.SystemIdle);
        blinkyTimer.Tick += new EventHandler(blinkNowEvent);
        blinkyTimer.Interval = TimeSpan.FromMilliseconds(1000);
        blinkyTimer.Start();
      }

      if (bEnable) {
        // enable blinky timer
        blinkyTimer.IsEnabled = true;
      } else if (!bEnable && (blinkyTimer != null)) {
        blinkyTimer.IsEnabled = false;
      }
    }

    private void blinkNowEvent(object source, EventArgs e) 
    {
      // change blink state
      // HACK - color code crashes
      blinkState = !blinkState;
      if (blinkState) {
        LinkStatusText.Foreground = Brushes.PaleVioletRed;
        LinkStatusText.Background = SystemColors.WindowBrush;
      } else {
        LinkStatusText.Foreground = SystemColors.WindowBrush;
        LinkStatusText.Background = Brushes.PaleVioletRed;
      }
    }

    internal void updateLinkStatus(PhiLink.PhiLinkState state) {

      bool newBlinky = false;

      switch (state) {
        case PhiLink.PhiLinkState.OFF:
          LinkStatusText.Text = " OFF ";
          break;
        case PhiLink.PhiLinkState.INITIALIZING:
          LinkStatusText.Text = " INITIALIZING ";
          newBlinky = true;
          break;
        case PhiLink.PhiLinkState.WAITING:
          LinkStatusText.Text = " WAITING ... ";
          newBlinky = true;
          break;
        case PhiLink.PhiLinkState.CONNECTED:
          LinkStatusText.Text = " CONNECTED ";
          break;
        case PhiLink.PhiLinkState.ERROR:
          LinkStatusText.Text = " ERROR ";
          newBlinky = true;
          break;
        case PhiLink.PhiLinkState.CANCELLED:
          LinkStatusText.Text = " CANCELLED ";
          newBlinky = true;
          break;
      }

      enableBlinkyTimer(newBlinky);
    }

    private void Window_Loaded(object sender, RoutedEventArgs e) {
      if (PhiGlobals.bInit == false) {
        // don't do again
        PhiGlobals.bInit = true;

        // one time initializations
        updateLinkStatus(PhiLink.PhiLinkState.OFF);
      }
    }
  }
}
