using Phi.Utils;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Phi {

  public partial class PhiJoint : IPhiController {

    //
    // CONST
    //

    // useful for conversion to uSecs
    const int USECS = 1;
    const int MSECS = 1000;
    const int SECS  = MSECS * 1000;

    // timeout value when moving - if joint position does not change after
    // this amount of time has elapsed, the movement times out and the
    // motor power is set to zero.
    public const UInt64 MOVE_TIMEOUT_USEC = 300 * MSECS;

    // ADC has 1024 resolution mapped to [0,1] so delta is approx 0.001
    // Values closer than the tolerance below are considered "equal"
    public const double JOINT_POS_TOLERANCE = 0.01;

    // prototype trajectory of joint - for more info, see MATLAB "JointTrajectory.m"
    const double FRAC_ACCEL = 0.2;
    const double FRAC_DECEL = 0.4;            // orig = 0.2;
    const double TRAJ_P1_X  = FRAC_ACCEL;
    const double TRAJ_P1_Y  = 0.112795;       // orig = 0.1;
    const double TRAJ_P2_X  = 1-FRAC_DECEL;
    const double TRAJ_P2_Y  = 0.774411;       // orig = 0.9;
    const double TRAJ_R1    = 0.233711;       // orig = 0.25;
    const double TRAJ_R2    = 0.467421;       // orig = 0.25;

    // log tree view node name (key)
    public const string LOG_NODENAME_JOINT_PREFIX = "logNode_Joint_";

    // when logging, enterring idle is deferred by this amount so we can watch
    // for overshoots
    const UInt64 DEFER_IDLE_USECS = 300 * MSECS;

    // this is how many frames ahead adaptive seek predicts
    const int PREDICTION_LOOKAHEAD_FRAMES = 1;
    const UInt64 PREDICTION_LOOKAHEAD_USECS = (UInt64) 
      (PREDICTION_LOOKAHEAD_FRAMES * PhiLink.DESIRED_SECS_PER_FRAME * SECS);

    //
    // VARS
    //

    private PhiLeg parent;
    public string name;

    private int motorIdx;
    private double PHI_motorPower;            // range [-1, 1]

    // used by seek functions
    private double targetPos;
    private UInt64 deferralEndTime;

    // used by adaptive seek function
    private double maxAbsPower;
    private double cmdStartPos;
    private double proto_deltaP;
    private double proto_deltaT;
    private PhiLog_Double log_recentPos;

    // from PHI state
    private double PHI_currPos;             // range [0, 1]

    // for timeouts
    private double lastMovePos;
    private UInt64 lastMoveTime;

    // limits
    private double minJointPos;
    private double maxJointPos;
    private double centerJointPos;

    // JOINT_STATE

    private enum JOINT_STATE {
      IDLE = 0,
      DEFERRING_IDLE,

      MOVE_WITH_TIMEOUT,
      SEEK_POS_WITH_TIMEOUT,
      ADAPTIVE_SEEK_POS_WITH_TIMEOUT,
    }

    private JOINT_STATE jointState;

    // log tree view node
    private System.Windows.Forms.TreeNode logTreeNode;

    // logs
    private PhiLogBase activeMoveLog = null;
    protected UInt64 cmdStartTime;
    protected UInt64 cmdCompletionTime;
    protected UInt64 cmdIdleTime;

    //
    // CODE
    //

    public PhiJoint(PhiLeg parent, string name, int motorIdx) {
      this.parent = parent;
      this.name = name;
      this.motorIdx = motorIdx;
      log_recentPos = null;

      // create log node
      logTreeNode = new System.Windows.Forms.TreeNode(LOG_NODENAME_JOINT_PREFIX + name);
      logTreeNode.Tag = null;
      logTreeNode.Text = "Joint " + name;
      parent.logTreeNode.Nodes.Add(logTreeNode);

      // allocate log of recent positions used by adaptive seek
      // Note: we keep track of the minimum of 4 plus something related
      //       the number of frames we are looking ahead

      int numExtra = PREDICTION_LOOKAHEAD_FRAMES - 1;
      log_recentPos = new PhiLog_Double(4 + numExtra, logName: "joint " + name + ": AS History");
      log_recentPos.setDataName("position");
      log_recentPos.setDataRange(0, 1);
    }

    public void reset() {
      setIdle();
      minJointPos = 0;
      maxJointPos = 1;
      centerJointPos = 0.5;
    }

    public void setIdle() {
      jointState = JOINT_STATE.IDLE;
      PHI_motorPower = 0;
      if (PhiGlobals.model != null) {
        cmdIdleTime  = PhiGlobals.model.PHI_currTime;
      }
    }

    public void deferIdle() {
      // stop motor but don't go into idle state just yet
      PHI_motorPower = 0;
      deferralEndTime = PhiGlobals.model.PHI_currTime + DEFER_IDLE_USECS;
      jointState = JOINT_STATE.DEFERRING_IDLE;
    }

    public int getIndex() {
      return motorIdx;
    }

    public double getPower() {
      return PHI_motorPower;
    }

    public double getPos() {
      return PHI_currPos;
    }

    public double getTargetPos() {
      return targetPos;
    }

    public double getMaxAbsPower() {
      return maxAbsPower;
    }

    public double getDuration() {
      return proto_deltaT;
    }

    public PhiLogBase getHistoryLog() {
      return log_recentPos;
    }

    public bool isIdle() {
      return (jointState == JOINT_STATE.IDLE);
    }

    public void causeTimeout() {
      commandComplete();
    }

    public void moveWithTimeout(double power) {

      jointState = JOINT_STATE.MOVE_WITH_TIMEOUT;
      double origPower = PHI_motorPower;
      PHI_motorPower = power;
      cmdStartTime = PhiGlobals.model.PHI_currTime;
      resetTimeout();

      // allocate log for debugging performance of simple movements

      if (true) {  // TODO: add debug flags here
        Debug.Assert(activeMoveLog == null, "activeLog != null ?!?!");
        activeMoveLog = new PhiLog_SimpleMove("Simple Move");
        (activeMoveLog as PhiLog_SimpleMove).Add(PhiGlobals.model.PHI_currTime, PHI_currPos, origPower);
      }

      // DEBUG
      // Console.WriteLine("moveWithTimeout() - time={0}", PhiGlobals.model.PHI_currTime);
    }

    public void seekWithTimeout(double power, double targetPos) {
      jointState = JOINT_STATE.SEEK_POS_WITH_TIMEOUT;
      double origPower = PHI_motorPower;
      PHI_motorPower = power;
      this.targetPos = targetPos;
      cmdStartTime = PhiGlobals.model.PHI_currTime;
      resetTimeout();

      // allocate log for debugging performance of simple seek
      if (true) {  // TODO: add debug flags here
        Debug.Assert(activeMoveLog == null, "activeLog != null ?!?!");
        activeMoveLog = new PhiLog_SimpleSeek("Simple Seek", targetPos);
        (activeMoveLog as PhiLog_SimpleSeek).Add(PhiGlobals.model.PHI_currTime, PHI_currPos, origPower);
      }
    }

    public void adaptiveSeekWithTimeout(double targetPos, double maxAbsPower, double msDuration = 0) {

      jointState = JOINT_STATE.ADAPTIVE_SEEK_POS_WITH_TIMEOUT;
      this.maxAbsPower = maxAbsPower;
      this.targetPos = targetPos;
      cmdStartTime = PhiGlobals.model.PHI_currTime;
      cmdStartPos = PHI_currPos;
      proto_deltaP = targetPos - cmdStartPos;

      double origPower = PHI_motorPower;
      PHI_motorPower = proto_deltaP > 0 ? PhiGlobals.testMotorPower : -PhiGlobals.testMotorPower;

      // figure out type
      if (msDuration != 0) {
        // duration specified - we want to complete this move in the specified time
        proto_deltaT = msDuration * MSECS;
      } else {
        // duration NOT specified - this means we want move as quickly as possible
        // TODO: not implemented yet
        throw new NotImplementedException();
      }
      
      // set up for timeouts
      resetTimeout();

      // allocate log for debugging performance of simple movements
      if (true) {  // TODO: add debug flags here
        Debug.Assert(activeMoveLog == null, "activeLog != null ?!?!");
        activeMoveLog = new PhiLog_AdaptiveSeek("Adaptive Seek", targetPos, (UInt64) (cmdStartTime + proto_deltaT));

        double predPos; UInt64 predTime;
        movePredict(out predPos, out predTime);
        double idealPos = protoTrajectory(predTime);

        (activeMoveLog as PhiLog_AdaptiveSeek).Add(
                              time: PhiGlobals.model.PHI_currTime,
                              position: PHI_currPos,
                              power: origPower,
                              predTime: predTime,
                              idealPos: idealPos,
                              interpPos: predPos);
      }
    }

    void resetTimeout() {
      // set up to time out if we don't move for a while
      lastMovePos = PHI_currPos;
      lastMoveTime = PhiGlobals.model.PHI_currTime;
    }

    void commandComplete(bool bDeferIdle = false) {

      cmdCompletionTime = PhiGlobals.model.PHI_currTime;

      if (bDeferIdle && (activeMoveLog != null)) {
        // logging  and deferred idle requested
        deferIdle();
      } else {
        setIdle();
      } 
    }

    //
    // JOINT STATE MACHINE
    //

    void jointStateMachine() {

      switch (jointState) {

        case JOINT_STATE.IDLE:

          if (activeMoveLog != null) {

            // was logging something - add to log window

            activeMoveLog.setTimeRange(cmdStartTime, cmdIdleTime, cmdCompletionTime);

            activeMoveLog.addToLogWindow(parentNode: logTreeNode,
                                         name: parent.name + name + "_SM_PerfLog",
                                         text: String.Format("T={0:F2}s: " + activeMoveLog.name, cmdStartTime / 1e6));

            activeMoveLog = null;
          }

          break;

        case JOINT_STATE.DEFERRING_IDLE:

          // when logging, seek commands use this state to defer entering
          // the idle state so we can keep logging to check for overshoots

          UInt64 currTime = PhiGlobals.model.PHI_currTime;

          if (activeMoveLog is PhiLog_SimpleMove) {
            (activeMoveLog as PhiLog_SimpleMove).Add(currTime, PHI_currPos, PHI_motorPower);
          } else if (activeMoveLog is PhiLog_SimpleSeek) {
            (activeMoveLog as PhiLog_SimpleSeek).Add(currTime, PHI_currPos, PHI_motorPower);
          } else if (activeMoveLog is PhiLog_AdaptiveSeek) {
            throw new NotImplementedException();
          } else {
            throw new NotImplementedException("unknown state in DEFERRING_IDLE");
          }

          if (currTime >= deferralEndTime) {
            deferralEndTime = 0;
            setIdle();
          }

          break;

        case JOINT_STATE.MOVE_WITH_TIMEOUT:

          // time out if haven't moved in a while
          checkForMoveTimeout();

          // log position and power
          if (activeMoveLog != null) {
            (activeMoveLog as PhiLog_SimpleMove).Add(PhiGlobals.model.PHI_currTime, PHI_currPos, PHI_motorPower);
          }

          // DEBUG
          // if (jointState != JOINT_STATE.MOVE_WITH_TIMEOUT) {
            // timeout
            //  Console.WriteLine("timeout in move simple - time={0}", PhiGlobals.model.PHI_currTime);
          // }

          break;

        case JOINT_STATE.SEEK_POS_WITH_TIMEOUT:
          jointStateMachine_simpleSeekWithTimeout();
          break;

        case JOINT_STATE.ADAPTIVE_SEEK_POS_WITH_TIMEOUT:
          jointStateMachine_adaptiveSeekWithTimeout();
          break;

      } // switch
    }

    void jointStateMachine_simpleSeekWithTimeout() {

      // log position and power
      if (activeMoveLog != null) {
        (activeMoveLog as PhiLog_SimpleSeek).Add(PhiGlobals.model.PHI_currTime, PHI_currPos, PHI_motorPower);
      }

      // check if done

      if (PhiGlobals.approxEq(PHI_currPos, targetPos, JOINT_POS_TOLERANCE)) {

        // reached target
        commandComplete(bDeferIdle: true);

        // DEBUG
        Console.WriteLine("*** SUCCESS when seeking to joint position={0:G3}", targetPos);

      } else {

        // not at target
        double deltaPos = PHI_currPos-targetPos;
        if (((PHI_motorPower > 0) && (deltaPos > 0)) ||
             ((PHI_motorPower < 0) && (deltaPos < 0))) {

          // overshot target - done
          commandComplete(bDeferIdle: true);

          // DEBUG
          Console.WriteLine("*** Overshoot occurred when seeking to joint position={0:G3}", targetPos);

        } else {

          // on way to target - time out if haven't moved in a while
          checkForMoveTimeout();

          // DEBUG
          //if (jointState != JOINT_STATE.SEEK_POS_WITH_TIMEOUT) {
            // timed out
            // Console.WriteLine("*** Timeout occurred when seeking to joint position={0:G3}", targetPos);
          //}
        }
      }
    }

    void jointStateMachine_adaptiveSeekWithTimeout() {

      UInt64 currTime = PhiGlobals.model.PHI_currTime;

      // log position and power
      if (activeMoveLog != null) {
        (activeMoveLog as PhiLog_SimpleSeek).Add(currTime, PHI_currPos, PHI_motorPower);
      }

      // check if done
      if (PhiGlobals.approxEq(PHI_currPos, targetPos, JOINT_POS_TOLERANCE)) {

        // reached target
        commandComplete(bDeferIdle: true);

        // DEBUG
        Console.WriteLine("*** SUCCESS when adapt_seeking to joint position={0:G3}", targetPos);

      } else {

        // not at target
        double deltaPos = PHI_currPos - targetPos;
        if ( ((PHI_motorPower > 0) && (deltaPos > 0)) ||
             ((PHI_motorPower < 0) && (deltaPos < 0))) {
          
          // overshot target
          commandComplete(bDeferIdle: true);

          // DEBUG
          Console.WriteLine("*** Overshoot occurred when adapt_seeking to joint position={0:G3}", targetPos);

        } else {

          // on way to target
          // time out if haven't moved in a while
          checkForMoveTimeout();

          // predict if our next position will follow our desired trajectory
          double predPos; UInt64 predTime;
          movePredict(out predPos, out predTime);
          double idealPos = protoTrajectory(predTime);
          double predError = predPos - idealPos;

          // adjust power to keep on track
          // TODO: the logic below is too simple (doesn't allow power to change sign, etc)
          double errorFrac = 1; // Math.Min(Math.Abs(predError), 0.05) / 0.05;
          if (Math.Sign(predError) * Math.Sign(PHI_motorPower) > 0) {
            // too fast
            PHI_motorPower /= 1.05 * errorFrac;
          } else {
            // too slow
            PHI_motorPower *= 1.2 * errorFrac;
            if (Math.Abs(PHI_motorPower) > maxAbsPower) {
              PHI_motorPower = Math.Sign(PHI_motorPower) * maxAbsPower;
            }
          }

          if (activeMoveLog != null) {

            (activeMoveLog as PhiLog_AdaptiveSeek).Add(
                                  time: PhiGlobals.model.PHI_currTime,
                                  position: PHI_currPos,
                                  power: PHI_motorPower,
                                  predTime: predTime,
                                  idealPos: idealPos,
                                  interpPos: predPos);
          }

          // DEBUG
          if (jointState != JOINT_STATE.ADAPTIVE_SEEK_POS_WITH_TIMEOUT) {
            Console.WriteLine("*** Timeout occurred when adapt_seeking to joint position={0:G3}", targetPos);
          }
        }
      }

      // DEBUG
      // Console.WriteLine("joint adaptive seek: curr={0:G3}, target={1:G3}, delta={2:G3}", 
      //                   PHI_currPos, targetPos, Math.Abs(PHI_currPos-targetPos));
    }

    void movePredict(out double predPos, out UInt64 predTime) {
       predTime = PhiGlobals.model.PHI_currTime + PREDICTION_LOOKAHEAD_USECS;
       predPos = CubicInterpolator.interpolateFromLog(log_recentPos, predTime);
    }

    void checkForMoveTimeout() {
      UInt64 currTime = PhiGlobals.model.PHI_currTime;
      if ((currTime - lastMoveTime) > MOVE_TIMEOUT_USEC) {
        if (PhiGlobals.approxEq(PHI_currPos, lastMovePos, JOINT_POS_TOLERANCE)) {
          // timeout has passed and joint did not move enough
          causeTimeout();
        } else {
          // moved enough - keep going
          resetTimeout();
        }
      }
    }

    // prototype trajectory used by adaptive seek

    public double protoTrajectory(double t) {

      double p = 0;

      // adjust t so complete at t = cmdStartTime + deltaT
      t = (t-cmdStartTime) / proto_deltaT;

      // find out which phase we're in
      if (t < 0) {
        // before curve
        p = 0;

      } else if (t < TRAJ_P1_X) {
        // accel phase
        double y = getCircleY(t, TRAJ_R1);
        p = TRAJ_R1 - y;

      } else if (t < TRAJ_P2_X) {
        // linear phase
        const double slope = (TRAJ_P2_Y - TRAJ_P1_Y) / (TRAJ_P2_X - TRAJ_P1_X);
        p = TRAJ_P1_Y + slope * (t - TRAJ_P1_X);

      } else if (t < 1) {
        // decel phase
        double y = getCircleY(1-t, TRAJ_R2);
        p = y + (1-TRAJ_R2);

      } else {
        // past end
        p = 1;
      }

      // adjust p so total move p = deltaP
      p = cmdStartPos + p * proto_deltaP;

      return p;
    }

    double getCircleY(double x, double r) {
      return Math.Sqrt(r*r - x*x);
    }

    //
    // IPHICONTROLLER INTERFACE
    //

    bool IPhiController.init(PhiStatePacket phiState, PhiCmdPacket phiCmds) {
      setIdle();
      return true;
    }

    void IPhiController.loadData(PhiStatePacket phiState) {
      PHI_currPos = phiState.getJointPos(motorIdx);
      log_recentPos.Add(PhiGlobals.model.PHI_currTime, PHI_currPos);
    }

    void IPhiController.step() {
      jointStateMachine();
    } // function step()

    void IPhiController.outputData(PhiCmdPacket phiCmds) {
      phiCmds.setMotorPower(motorIdx, PHI_motorPower);
    }

    void IPhiController.shutdown() {
    }

  } // PhiJoint class

} // namespace
