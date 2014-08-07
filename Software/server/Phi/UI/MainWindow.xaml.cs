using Phi.UI;
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

namespace Phi {

  public partial class MainWindow : Window {

    //
    // VARS
    //

    private DispatcherTimer updateTimer;
    private DispatcherTimer blinkyTimer;

    private bool blinkState = false;
    private Task phiLinkTask;

    private bool bPhiLinkChanged = false;

    //
    // CODE
    //

    public MainWindow() {

      InitializeComponent();

      // save ref
      PhiGlobals.mainWindow = this;
    }

    private void Window_Loaded(object sender, RoutedEventArgs e) {

      if (PhiGlobals.bInit == false) {
        // don't do again
        PhiGlobals.bInit = true;

        //
        // One time initializations
        //

        // init gyro controls
        PieCtrl_Pitch.titleName.Text = "Pitch";
        PieCtrl_Yaw.titleName.Text = "Yaw";
        PieCtrl_Roll.titleName.Text = "Roll";

        // init leg controls

        LF_Leg.legLabel.Content = "Left Front (A)";
        LF_Leg.hipJoint.label.Content = "H";
        LF_Leg.thighJoint.label.Content = "T";
        LF_Leg.shinJoint.label.Content = "S";

        RF_Leg.legLabel.Content = "Right Front (B)";
        RF_Leg.hipJoint.label.Content = "H";
        RF_Leg.thighJoint.label.Content = "T";
        RF_Leg.shinJoint.label.Content = "S";

        LB_Leg.legLabel.Content = "Left Back (C)";
        LB_Leg.hipJoint.label.Content = "H";
        LB_Leg.thighJoint.label.Content = "T";
        LB_Leg.shinJoint.label.Content = "S";

        RB_Leg.legLabel.Content = "Right Back (D)";
        RB_Leg.hipJoint.label.Content = "H";
        RB_Leg.thighJoint.label.Content = "T";
        RB_Leg.shinJoint.label.Content = "S";

        // enable timer that refreses UI
        enableUpdateTimer(true);

        // start Phi Link listener
        startPhiLink();
      }
    }

    private void startPhiLink() {
      // start listener task
      phiLinkTask = new Task(PhiLink.phiLinkTask);
      phiLinkTask.Start();
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

    public void notifyPhiLinkChanged() {
      // called by other threads to tell the UI thread that the phiLink status has changed
      bPhiLinkChanged = true;
    }

    private void updateLinkStatus() {

      bool newBlinky = false;

      switch (PhiLink.linkStatus) {
        case PhiLink.PHI_LINK_STATUS.LINK_OFF:
          LinkStatusText.Text = " OFF ";
          break;
        case PhiLink.PHI_LINK_STATUS.LINK_STARTED:
          LinkStatusText.Text = " INITIALIZING ";
          newBlinky = true;
          break;
        case PhiLink.PHI_LINK_STATUS.LINK_CONNECTING:
          LinkStatusText.Text = " WAITING ... ";
          newBlinky = true;
          break;
        case PhiLink.PHI_LINK_STATUS.LINK_CONNECTED:
          LinkStatusText.Text = " CONNECTED ";
          break;
        case PhiLink.PHI_LINK_STATUS.LINK_ERROR:
          LinkStatusText.Text = " ERROR ";
          newBlinky = true;
          break;
        case PhiLink.PHI_LINK_STATUS.LINK_CLOSED:
          LinkStatusText.Text = " CLOSED ";
          newBlinky = true;
          break;
      }

      enableBlinkyTimer(newBlinky);
    }

    private void enableUpdateTimer(bool bEnable) {

      if (updateTimer == null) {
        updateTimer = new DispatcherTimer(DispatcherPriority.SystemIdle);
        updateTimer.Tick += new EventHandler(updateUI_timerEvent);
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

    private void updateUI_timerEvent(object source, EventArgs e) {

      // update link status if it has changed
      if (bPhiLinkChanged == true) {
        // changed
        bPhiLinkChanged = false;
        updateLinkStatus();
      }

      // update PHI state UI

      LinkFrameRateText.Text = PhiLink.getAvgFrameRate().ToString("F1") + " / " + PhiLink.DESIRED_LOOP_FPS + " Hz";
      AvgIdleText.Text = (PhiLink.getAvgIdle() * 100).ToString("F1") + " %";
      ctlNumLoops.Text  = PhiLink.getLoopCount().ToString();
      ctlPacketId.Text = PhiLink.getLastPacketID().ToString();
      ctlSleepError.Text = (PhiLink.getSleepError() * 1000).ToString("F1");

      // HACK - disabled for now
      // PieCtrl_Pitch.update(PhiLink.getGyroAccumPitch());
      // PieCtrl_Yaw.update(PhiLink.getGyroAccumYaw());
      // PieCtrl_Roll.update(PhiLink.getGyroAccumRoll());

      PhiModel m = PhiGlobals.model;

      const int A = PhiModel.LEG_A_IDX;
      const int B = PhiModel.LEG_B_IDX;
      const int C = PhiModel.LEG_C_IDX;
      const int D = PhiModel.LEG_D_IDX;

      const int H = PhiLeg.HIP_IDX;
      const int T = PhiLeg.THIGH_IDX;
      const int S = PhiLeg.SHIN_IDX;

      (LegGrid.FindName("LF_Leg") as LegBox).update(
        m.legs[A].joints[H].getPower(), m.legs[A].joints[H].getPos(),
        m.legs[A].joints[T].getPower(), m.legs[A].joints[T].getPos(),
        m.legs[A].joints[S].getPower(), m.legs[A].joints[S].getPos());

      (LegGrid.FindName("RF_Leg") as LegBox).update(
        m.legs[B].joints[H].getPower(), m.legs[B].joints[H].getPos(),
        m.legs[B].joints[T].getPower(), m.legs[B].joints[T].getPos(),
        m.legs[B].joints[S].getPower(), m.legs[B].joints[S].getPos());

      (LegGrid.FindName("LB_Leg") as LegBox).update(
        m.legs[C].joints[H].getPower(), m.legs[C].joints[H].getPos(),
        m.legs[C].joints[T].getPower(), m.legs[C].joints[T].getPos(),
        m.legs[C].joints[S].getPower(), m.legs[C].joints[S].getPos());

      (LegGrid.FindName("RB_Leg") as LegBox).update(
        m.legs[D].joints[H].getPower(), m.legs[D].joints[H].getPos(),
        m.legs[D].joints[T].getPower(), m.legs[D].joints[T].getPos(),
        m.legs[D].joints[S].getPower(), m.legs[D].joints[S].getPos());

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
      throw new NotImplementedException("reset gyro");
      // PhiLink.resetGyroAccum();
    }

    //
    // FILE MENU
    //

    private void menuFile_Exit(object sender, RoutedEventArgs e) {
      // TODO HACK - we should create an action to safe the legs and
      // when it is done we should close
      this.Close();
    }

    //
    // ACTIONS MENU
    //

    private StartupController getStartupController() {
      IPhiController controller = PhiLink.getActiveController();
      if (controller == null) {
        MessageBox.Show("No controller. (Connect with PHI first.)");

      } else if (controller.GetType() != typeof(StartupController)) {
        MessageBox.Show("Wrong controller type! Operation only valid with StartupController.");
        controller = null;
      }
      return (controller as StartupController);
    }

    private void menuActions_runStartupTests(object sender, RoutedEventArgs e) {
      // run startup tests ONCE
      StartupController controller = getStartupController();
      if (controller != null) {
        controller.startTests();
      }
    }

    private void menuActions_runStartupTestsForever(object sender, RoutedEventArgs e) {
      // run startup tests FOREVER (use Abort All to quie)
      StartupController controller = getStartupController();
      if (controller != null) {
        controller.startTests(bRepeatForever: true);
      }
    }

    private void menuActions_ExtendFlat(object sender, RoutedEventArgs e) {

      // have user select leg
      DlgSelectLeg dlg = new DlgSelectLeg();
      dlg.Owner = this;
      dlg.ShowDialog();

      if (dlg.DialogResult == true) {
        StartupController controller = getStartupController();
        if (controller != null) {
          controller.extendLegFlat(dlg.getLegIdx());
        }
      }
    }

    private void menuActions_LegsToSafe(object sender, RoutedEventArgs e) {
    }

    private void menuActions_CrudeStandUp(object sender, RoutedEventArgs e) {
    }

    private void menuActions_TestLegs(object sender, RoutedEventArgs e) {
    }

    private void menuActions_DumpActions(object sender, RoutedEventArgs e) {
      PhiGlobals.model.dumpActionHierarchy();
    }

    private void menuActions_AbortAll(object sender, RoutedEventArgs e) {
      Console.WriteLine("* Aborting all actions!");
      PhiGlobals.model.abortAllActionsAndDump();
    }

  }
}
