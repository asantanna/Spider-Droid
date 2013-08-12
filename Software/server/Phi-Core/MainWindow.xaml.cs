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
    private static DispatcherTimer updateTimer;

    //
    // CODE
    //

    public MainWindow() {

      InitializeComponent();

      // save ref
      PhiGlobals.mainWindow = this;
    }

    async private void startPhiLink() {

      // start listener (ASYNC)
      Task<NetworkStream> listenTask =  PhiLink.startListenerAsync();

      // wait for connection (await causes immediate suspend)
      NetworkStream phiStream = await listenTask;

      // CONNECTED - start comm loop
      if (phiStream != null) {
        PhiLink.startCommLoop(phiStream);
      } else {
        // no stream?
        updateLinkStatus(PhiLink.PhiLinkState.LINK_ERROR);
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

    private void blinkNowEvent(object source, EventArgs e) {
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
        case PhiLink.PhiLinkState.LINK_OFF:
          LinkStatusText.Text = " OFF ";
          break;
        case PhiLink.PhiLinkState.LINK_STARTED:
          LinkStatusText.Text = " INITIALIZING ";
          newBlinky = true;
          break;
        case PhiLink.PhiLinkState.LINK_CONNECTING:
          LinkStatusText.Text = " WAITING ... ";
          newBlinky = true;
          break;
        case PhiLink.PhiLinkState.LINK_CONNECTED:
          LinkStatusText.Text = " CONNECTED ";
          enableUpdateTimer(true);
          break;
        case PhiLink.PhiLinkState.LINK_ERROR:
          LinkStatusText.Text = " ERROR ";
          newBlinky = true;
          enableUpdateTimer(false);
          break;
        case PhiLink.PhiLinkState.LINK_CLOSED:
          LinkStatusText.Text = " CANCELLED ";
          newBlinky = true;
          enableUpdateTimer(false);
          break;
      }

      enableBlinkyTimer(newBlinky);
    }

    private void Window_Loaded(object sender, RoutedEventArgs e) {
      if (PhiGlobals.bInit == false) {
        // don't do again
        PhiGlobals.bInit = true;

        // one time initializations
        updateLinkStatus(PhiLink.PhiLinkState.LINK_OFF);
        updateNowEvent(null, null);

        // start Phi Link listener
        startPhiLink();
      }
    }

    private void enableUpdateTimer(bool bEnable) {

      if (updateTimer == null) {
        updateTimer = new DispatcherTimer(DispatcherPriority.SystemIdle);
        updateTimer.Tick += new EventHandler(updateNowEvent);
        updateTimer.Interval = TimeSpan.FromMilliseconds(500);
        updateTimer.Start();
      }

      if (bEnable) {
        // enable update timer
        updateTimer.IsEnabled = true;
      } else if (!bEnable && (updateTimer != null)) {
        updateTimer.IsEnabled = false;
      }
    }

    private void updateNowEvent(object source, EventArgs e) {
      // update UI
      LinkFrameRateText.Text = PhiLink.getAvgFrameRate().ToString("F1") + " / " + PhiLink.DESIRED_LOOP_FPS + " Hz";
      AvgIdleText.Text = (PhiLink.getAvgIdle() * 100).ToString("F1") + " %";

      drawGyroIndicator("pitch", PhiLink.getGyroAccumPitch());
      drawGyroIndicator("yaw", PhiLink.getGyroAccumYaw());
      drawGyroIndicator("roll", PhiLink.getGyroAccumRoll());
    }

    private void drawGyroIndicator(string prefix, double degrees) {

      Canvas c = (Canvas) FindName(prefix + "Canvas");
      double radius = c.ActualHeight / 2;

      // note: degrees is [-180,1]
      double endAngle = Math.PI * degrees / 180;

      ArcSegment arc = (ArcSegment) FindName(prefix + "Arc");
      double xComp = radius * Math.Cos(endAngle);
      double yComp = radius * Math.Sin(endAngle);
      if (yComp >= 0) {
        arc.SweepDirection = SweepDirection.Clockwise;
      } else {
        arc.SweepDirection = SweepDirection.Counterclockwise;
      }
      arc.Point = new Point(radius + yComp, radius - xComp);

      Label label = (Label) FindName(prefix + "Value");
      label.Content = degrees.ToString("F1") + " \u00B0";
    }

    private void BtnResetGyro_Click(object sender, RoutedEventArgs e) {
      PhiLink.resetGyroAccum();
    }

  }
}
