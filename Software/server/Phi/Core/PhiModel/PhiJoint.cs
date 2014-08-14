﻿using Phi.Utils;
using System;
using System.Collections.Generic;
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

    // motor power used for startup tests
    public const double TEST_MOTOR_POWER = 0.4;  

    // prototype trajectory of joint - for more info, see MATLAB "JointTrajectory.m"
    const double TRAJ_P1_X = 0.2;
    const double TRAJ_P1_Y = 0.1;
    const double TRAJ_P2_X = 0.8;
    const double TRAJ_P2_Y = 0.9;
    const double TRAJ_R1   = 0.25;
    const double TRAJ_R2   = 0.25;

    //
    // VARS
    //

    // JOINT_STATE

    private enum JOINT_STATE {
      IDLE = 0,
      TIMEOUT_IDLE,
      MOVE_FOREVER,
      MOVE_WITH_TIMEOUT,
      SEEK_POS_WITH_TIMEOUT,
      ADAPTIVE_SEEK_POS_WITH_TIMEOUT,
    }

    private JOINT_STATE jointState;

    private string name;
    private int motorIdx;
    private double motorPower;            // range [-1, 1]

    // used by seek functions
    private double targetPos;

    // used by adaptive seek function
    private double maxAbsPower;
    private double startTime;
    private double deltaP;
    private double deltaT;
    private PhiLog_Double history;

    // from PHI state
    private double PHI_currPos;           // range [0, 1]

    // for timeouts
    private double lastMovePos;
    private UInt64 lastMoveTime;

    // limits
    private double minJointPos;
    private double maxJointPos;
    private double centerJointPos;

    // DEBUG
    private double minDeltaPos;        // used for debugging tolerances

    //
    // CODE
    //

    public PhiJoint(string name, int motorIdx) {
      this.name = name;
      this.motorIdx = motorIdx;
      history = new PhiLog_Double(4, logName: "joint " + name + ": AS History", dataName: "Position");
    }

    public void reset() {
      setIdle();
      minJointPos = 0;
      maxJointPos = 1;
      centerJointPos = 0.5;
    }

    public void setIdle() {
      jointState = JOINT_STATE.IDLE;
      motorPower = 0;
    }

    public int getIndex() {
      return motorIdx;
    }

    public double getPower() {
      return motorPower;
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
      return deltaT;
    }

    public bool isIdle() {
      return (jointState == JOINT_STATE.IDLE) || (jointState == JOINT_STATE.TIMEOUT_IDLE);
    }

    public void causeTimeout() {
      jointState = JOINT_STATE.TIMEOUT_IDLE;
      motorPower = 0;
    }

    public void moveWithTimeout(double power) {
      jointState = JOINT_STATE.MOVE_WITH_TIMEOUT;
      motorPower = power;
      resetTimeout();
    }

    public void seekWithTimeout(double power, double targetPos) {
      jointState = JOINT_STATE.SEEK_POS_WITH_TIMEOUT;
      motorPower = power;
      this.targetPos = targetPos;
      resetTimeout();
      // DEBUG
      minDeltaPos = 1.0;
    }

    public void adaptiveSeekWithTimeout(double targetPos, double absMaxPower, double msDuration = 0) {

      jointState = JOINT_STATE.ADAPTIVE_SEEK_POS_WITH_TIMEOUT;
      this.maxAbsPower = absMaxPower;
      this.targetPos = targetPos;
      deltaP = targetPos - PHI_currPos;
      startTime = PhiGlobals.model.getPhiTime();

      // figure out type
      if (msDuration != 0) {
        // duration specified - we want to complete this move in the specified time
        this.deltaT = msDuration * MSECS;
      } else {
        // duration NOT specified - this means we want move as quickly as possible
        throw new NotImplementedException();
      }
      
      resetTimeout();

      // DEBUG
      minDeltaPos = 1.0;
    }

    void resetTimeout() {
      // set up to time out if we don't move for a while
      lastMovePos = PHI_currPos;
      lastMoveTime = PhiGlobals.model.getPhiTime();
    }

    void jointStateMachine() {
      switch (jointState) {

        case JOINT_STATE.IDLE:
          // do nothing
          break;

        case JOINT_STATE.TIMEOUT_IDLE:
          // do nothing - this is an idle state ENTERED because of a timeout
          break;

        case JOINT_STATE.MOVE_FOREVER:
          // do nothing
          break;

        case JOINT_STATE.MOVE_WITH_TIMEOUT:
          // time out if haven't moved in a while
          checkForMoveTimeout();
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

      double deltaPos = PHI_currPos-targetPos;

      // DEBUG
      if (Math.Abs(deltaPos) < Math.Abs(minDeltaPos)) {
        minDeltaPos = deltaPos;
      }

      // check if done
      if (PhiGlobals.approxEq(PHI_currPos, targetPos, JOINT_POS_TOLERANCE)) {

        // reached target - done
        setIdle();

        // DEBUG
        Console.WriteLine("*** SUCCESS when seeking to joint position={0:G3}, minDelta={1:G3}",
          targetPos, minDeltaPos);

      } else {

        // not at target
        if ( ((motorPower > 0) && (deltaPos > 0)) ||
             ((motorPower < 0) && (deltaPos < 0))) {

          // overshot target - done
          setIdle();

          // DEBUG
          Console.WriteLine("*** Overshoot occurred when seeking to joint position={0:G3}, minDelta={1:G3}",
            targetPos, minDeltaPos);

        } else {

          // on way to target
          // time out if haven't moved in a while
          checkForMoveTimeout();

          // DEBUG
          if (jointState == JOINT_STATE.TIMEOUT_IDLE) {
            Console.WriteLine("*** Timeout occurred when seeking to joint position={0:G3}, minDelta={1:G3}",
              targetPos, minDeltaPos);
          }
        }
      }

      // DEBUG
      // Console.WriteLine("joint seek: curr={0:G3}, target={1:G3}, delta={2:G3}", 
      //                   PHI_currPos, targetPos, Math.Abs(PHI_currPos-targetPos));
    }

    void jointStateMachine_adaptiveSeekWithTimeout() {

      double deltaPos = PHI_currPos-targetPos;

      // DEBUG
      if (Math.Abs(deltaPos) < Math.Abs(minDeltaPos)) {
        minDeltaPos = deltaPos;
      }

      // check if done
      if (PhiGlobals.approxEq(PHI_currPos, targetPos, JOINT_POS_TOLERANCE)) {

        // reached target - done
        setIdle();

        // DEBUG
        Console.WriteLine("*** SUCCESS when adapt_seeking to joint position={0:G3}, minDelta={1:G3}",
          targetPos, minDeltaPos);

      } else {

        // not at target

        if ( ((motorPower > 0) && (deltaPos > 0)) ||
             ((motorPower < 0) && (deltaPos < 0))) {

          // overshot target - done
          setIdle();

          // DEBUG
          Console.WriteLine("*** Overshoot occurred when adapt_seeking to joint position={0:G3}, minDelta={1:G3}",
            targetPos, minDeltaPos);

        } else {

          // on way to target
          // time out if haven't moved in a while
          checkForMoveTimeout();

          // predict if our next position will follow our desired trajectory
          double nextT = PhiGlobals.model.getPhiTime() + (PhiLink.DESIRED_SECS_PER_LOOP * SECS);

          // TODO - stopped here to work on logs
          // double trajDelta = history.predictNext() - protoTrajectory(nextT - startTime);

          // DEBUG
          if (jointState == JOINT_STATE.TIMEOUT_IDLE) {
            Console.WriteLine("*** Timeout occurred when adapt_seeking to joint position={0:G3}, minDelta={1:G3}",
              targetPos, minDeltaPos);
          }
        }
      }

      // DEBUG
      // Console.WriteLine("joint adaptive seek: curr={0:G3}, target={1:G3}, delta={2:G3}", 
      //                   PHI_currPos, targetPos, Math.Abs(PHI_currPos-targetPos));
    }

    void checkForMoveTimeout() {
      UInt64 currTime = PhiGlobals.model.getPhiTime();
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

      // adjust t so complete at t = deltaT
      t = t / deltaT;

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
      p = p * deltaP;

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
    }

    void IPhiController.step() {
      jointStateMachine();
    } // function step()

    void IPhiController.outputData(PhiCmdPacket phiCmds) {
      phiCmds.setMotorPower(motorIdx, motorPower);
    }

    void IPhiController.shutdown() {
    }

  } // PhiJoint class

} // namespace
