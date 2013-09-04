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

        //
        // One time initializations
        //

        updateLinkStatus(PhiLink.PhiLinkState.LINK_OFF);

        // start Phi Link listener
        startPhiLink();

        // init gyro controls
        PieCtrl_Pitch.titleName.Text = "Pitch";
        PieCtrl_Yaw.titleName.Text = "Yaw";
        PieCtrl_Roll.titleName.Text = "Roll"; 

        // init leg controls

        LF_Leg.legLabel.Content = "Left Front";
        LF_Leg.hipJoint.label.Content = "H";
        LF_Leg.thighJoint.label.Content = "T";
        LF_Leg.kneeJoint.label.Content = "K";

        RF_Leg.legLabel.Content = "Right Front";
        RF_Leg.hipJoint.label.Content = "H";
        RF_Leg.thighJoint.label.Content = "T";
        RF_Leg.kneeJoint.label.Content = "K";

        LB_Leg.legLabel.Content = "Left Back";
        LB_Leg.hipJoint.label.Content = "H";
        LB_Leg.thighJoint.label.Content = "T";
        LB_Leg.kneeJoint.label.Content = "K";

        RB_Leg.legLabel.Content = "Right Back";
        RB_Leg.hipJoint.label.Content = "H";
        RB_Leg.thighJoint.label.Content = "T";
        RB_Leg.kneeJoint.label.Content = "K";

        // refresh UI
        updateNowEvent(null, null);
      }
    }

    private void enableUpdateTimer(bool bEnable) {

      if (updateTimer == null) {
        updateTimer = new DispatcherTimer(DispatcherPriority.SystemIdle);
        updateTimer.Tick += new EventHandler(updateNowEvent);
        updateTimer.Interval = TimeSpan.FromMilliseconds(100);
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
      ctlNumLoops.Text  = PhiLink.getLoopCount().ToString();
      ctlPacketId.Text = PhiLink.getLastPacketID().ToString();

      PieCtrl_Pitch.update(PhiLink.getGyroAccumPitch());
      PieCtrl_Yaw.update(PhiLink.getGyroAccumYaw());
      PieCtrl_Roll.update(PhiLink.getGyroAccumRoll());

      (LegGrid.FindName("LF_Leg") as LegBox).update(
        -0.5, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_LFH),
         0.2, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_LFT),
         0.9, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_LFK));

      (LegGrid.FindName("RF_Leg") as LegBox).update(
        -1.0, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_RFH),
        -0.9, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_RFT),
        -0.8, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_RFK));

      (LegGrid.FindName("LB_Leg") as LegBox).update(
         1.0,  PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_LBH),
         0.66, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_LBT),
         0.33, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_LBK));

      (LegGrid.FindName("RB_Leg") as LegBox).update(
        -1.0,  PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_RBH),
        -0.66, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_RBT),
        -0.33, PhiLink.getJointPos((int) PhiLink.MOTOR_IDX.IDX_RBK));

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
