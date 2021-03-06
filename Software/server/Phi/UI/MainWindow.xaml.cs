﻿using Phi.UI;
using Phi.Utils;
using Eve;

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

    private bool bForeverMode = false;

    //
    // CODE
    //

    public MainWindow() {

      InitializeComponent();

      // save ref
      PhiGlobals.mainWindow = this;

      // create hidden log windows and call show() so thread gets created
      // (needed by Invoke) and then hide it immediately
      PhiGlobals.logForm = new LogForm();
      PhiGlobals.logForm.Show();
      PhiGlobals.logForm.Hide();
    }

    private void Window_Loaded(object sender, RoutedEventArgs e) {

      if (PhiGlobals.bInit == false) {
        // don't do again
        PhiGlobals.bInit = true;

        //
        // One time initializations
        //

        // create model
        PhiGlobals.model = new PhiModel();

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

      LinkFrameRateText.Text = PhiLink.getAvgFrameRate().ToString("F1") + " / " + PhiLink.DESIRED_FPS + " Hz";
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
      StartupController controller = getStartupController();
      if (controller != null) {
        controller.startTests(bRepeatForever: bForeverMode);
      }
    }

    //
    // LEG COMMANDS
    //

    // EXTEND LEG
    
    private void extendLegX(int legIdx) {
      StartupController controller = getStartupController();
      if (controller != null) {
        controller.extendLegFull(legIdx, bForeverMode ? PA_Group.REPEAT_FOREVER : 1);
      }
    }

    private void menuActions_ExtendLegFull_A(object sender, RoutedEventArgs e) {
      extendLegX(PhiModel.LEG_A_IDX);
    }

    private void menuActions_ExtendLegFull_B(object sender, RoutedEventArgs e) {
      extendLegX(PhiModel.LEG_B_IDX);
    }

    private void menuActions_ExtendLegFull_C(object sender, RoutedEventArgs e) {
      extendLegX(PhiModel.LEG_C_IDX);
    }

    private void menuActions_ExtendLegFull_D(object sender, RoutedEventArgs e) {
      extendLegX(PhiModel.LEG_D_IDX);
    }

    private void menuActions_ExtendLegFull_All(object sender, RoutedEventArgs e) {
      for (int i = PhiModel.LEG_A_IDX ; i <= PhiModel.LEG_D_IDX ; i++) {
        extendLegX(i);
      }
    }

    // RETRACT LEG

    private void retractLegX(int legIdx) {
      StartupController controller = getStartupController();
      if (controller != null) {
        controller.retractLegFull(legIdx, bForeverMode ? PA_Group.REPEAT_FOREVER : 1);
      }
    }

    private void menuActions_RetractLegFull_A(object sender, RoutedEventArgs e) {
      retractLegX(PhiModel.LEG_A_IDX);
    }

    private void menuActions_RetractLegFull_B(object sender, RoutedEventArgs e) {
      retractLegX(PhiModel.LEG_B_IDX);
    }

    private void menuActions_RetractLegFull_C(object sender, RoutedEventArgs e) {
      retractLegX(PhiModel.LEG_C_IDX);
    }

    private void menuActions_RetractLegFull_D(object sender, RoutedEventArgs e) {
      retractLegX(PhiModel.LEG_D_IDX);
    }

    private void menuActions_RetractLegFull_All(object sender, RoutedEventArgs e) {
      for (int i = PhiModel.LEG_A_IDX ; i <= PhiModel.LEG_D_IDX ; i++) {
        retractLegX(i);
      }
    }

    // CENTER JOINT

    private void centerJointsX(int legIdx) {
      StartupController controller = getStartupController();
      if (controller != null) {
        controller.centerJoints(legIdx, bForeverMode ? PA_Group.REPEAT_FOREVER : 1);
      }
    }
    
    private void menuActions_CenterJoints_A(object sender, RoutedEventArgs e) {
      centerJointsX(PhiModel.LEG_A_IDX);
    }

    private void menuActions_CenterJoints_B(object sender, RoutedEventArgs e) {
      centerJointsX(PhiModel.LEG_B_IDX);
    }

    private void menuActions_CenterJoints_C(object sender, RoutedEventArgs e) {
      centerJointsX(PhiModel.LEG_C_IDX);
    }
    
    private void menuActions_CenterJoints_D(object sender, RoutedEventArgs e) {
      centerJointsX(PhiModel.LEG_D_IDX);
    }
    
    private void menuActions_CenterJoints_All(object sender, RoutedEventArgs e) {
      for (int i = PhiModel.LEG_A_IDX ; i <= PhiModel.LEG_D_IDX ; i++) {
        centerJointsX(i);
      }
    }

    //
    // TEST LEG
    //
    
    private void testLegX(int legIdx) {
      StartupController controller = getStartupController();
      if (controller != null) {
        controller.testLeg(legIdx, bForeverMode ? PA_Group.REPEAT_FOREVER : 1);
      }
    }

    private void menuActions_TestLeg_A(object sender, RoutedEventArgs e) {
      testLegX(PhiModel.LEG_A_IDX);
    }

    private void menuActions_TestLeg_B(object sender, RoutedEventArgs e) {
      testLegX(PhiModel.LEG_B_IDX);
    }

    private void menuActions_TestLeg_C(object sender, RoutedEventArgs e) {
      testLegX(PhiModel.LEG_C_IDX);
    }

    private void menuActions_TestLeg_D(object sender, RoutedEventArgs e) {
      testLegX(PhiModel.LEG_D_IDX);
    }

    private void menuActions_TestLeg_All(object sender, RoutedEventArgs e) {
      for (int i = PhiModel.LEG_A_IDX ; i <= PhiModel.LEG_D_IDX ; i++) {
        testLegX(i);
      }
    }

    // options

    private void menuAction_runForever(object sender, RoutedEventArgs e) {
      if ((sender as MenuItem).IsChecked) {
        bForeverMode = true;
      } else {
        bForeverMode = false;
      }
    }

    private void setTestPower(double power, object sender) {
      MenuItem selItem = (sender as MenuItem);
      MenuItem parent = (selItem.Parent as MenuItem);

      foreach (MenuItem item in parent.Items) {
        if (item == selItem) {
          item.IsChecked = true;
        } else {
          item.IsChecked = false;
        }
      }

      // set global value
      PhiGlobals.testMotorPower = power;
    }
    
    private void menuAction_powerTo20(object sender, RoutedEventArgs e)  { setTestPower(20.0/100,  sender); }
    private void menuAction_powerTo30(object sender, RoutedEventArgs e)  { setTestPower(30.0/100,  sender); }
    private void menuAction_powerTo40(object sender, RoutedEventArgs e)  { setTestPower(40.0/100,  sender); }
    private void menuAction_powerTo50(object sender, RoutedEventArgs e)  { setTestPower(50.0/100,  sender); }
    private void menuAction_powerTo75(object sender, RoutedEventArgs e)  { setTestPower(75.0/100,  sender); }
    private void menuAction_powerTo100(object sender, RoutedEventArgs e) { setTestPower(100.0/100, sender); }

    //
    // DUMP / ABORT ACTIONS
    //

    private void menuActions_DumpActions(object sender, RoutedEventArgs e) {
      PhiGlobals.model.dumpActionHierarchy();
    }

    private void menuActions_AbortAll(object sender, RoutedEventArgs e) {
      Console.WriteLine("*** Aborting all actions!");
      PhiGlobals.model.abortAllActionsAndDump();
    }

    private void menuLogs_showWindow(object sender, RoutedEventArgs e) {
      PhiGlobals.logForm.Show();
    }

    private void menuDebug_createTestLog(object sender, RoutedEventArgs e) {

      // create a test node
      System.Windows.Forms.TreeNode testNode = new System.Windows.Forms.TreeNode("TestLog");
      testNode.Text = "Test Log";

      // create a log for it
      PhiLog_Double log = new PhiLog_Double(5, logName: "test log");
      log.Add(1000, 10);
      log.Add(2000, 25);
      log.Add(3000, 32);
      log.Add(4000, 18);
      log.Add(5000, 2);
      testNode.Tag = log;

      // add to window
      log.addToLogWindow(PhiGlobals.logForm.logTreeRootNode, "test-log", "Test Log");
    }

    private void menuDebug_runClassTests(object sender, RoutedEventArgs e) {
      PhiGlobals.runClassTests();
    }

    private void MenuItem_Click(object sender, RoutedEventArgs e) {
      EveGlobals.runTests(null);
    }
    
  }
}


//
// SAVE TEMPORARILY
//


    /*
     SAVE

      // have user select leg
      DlgSelectLeg dlg = new DlgSelectLeg();
      dlg.Owner = this;
      dlg.ShowDialog();


    private void menuActions_LegsToSafe(object sender, RoutedEventArgs e) {
    }

    private void menuActions_CrudeStandUp(object sender, RoutedEventArgs e) {
    }

    */

