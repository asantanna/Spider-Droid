using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  public class PhiJoint : IPhiController {

    //
    // CONST
    //

    // timeout value when moving - if joint position does not change after
    // this amount of time has elapsed, the movement times out and the
    // motor power is set to zero.
    public const UInt64 MOVE_TIMEOUT_USEC = 500 * 1000;

    // ADC has 1024 resolution mapped to [0,1] so delta approx 0.001
    // Values closer than the tolerance below are considered "equal"
    public const double JOINT_POS_TOLERANCE = 0.01;

    // motor power used for startup tests
    public const double TEST_MOTOR_POWER = 0.3;  

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
    }

    private JOINT_STATE jointState;

    private int motorIdx;
    private double motorPower;            // range [-1, 1]
    private double targetPos;         

    // from PHI state
    private double PHI_currPos;           // range [0, 1]

    // for timeouts
    private double lastMovePos;
    private UInt64 lastMoveTime;

    // limits
    private double minJointPos;
    private double maxJointPos;
    private double centerJointPos;

    //
    // CODE
    //

    public PhiJoint(int motorIdx) {
      this.motorIdx = motorIdx;
    }

    public void reset() {
      setIdle();
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

    public bool isIdle() {
      return (jointState == JOINT_STATE.IDLE) || (jointState == JOINT_STATE.TIMEOUT_IDLE);
    }

    public void setTimedOut() {
      jointState = JOINT_STATE.TIMEOUT_IDLE;
      motorPower = 0;
    }

    public void moveWithTimeout(double power) {
      jointState = JOINT_STATE.MOVE_WITH_TIMEOUT;
      motorPower = power;
      // we want to time out if we don't move for a while
      lastMovePos = PHI_currPos;
      lastMoveTime = PhiGlobals.model.getPhiTime();
    }

    public void moveTo(double power, double targetPos) {
      jointState = JOINT_STATE.SEEK_POS_WITH_TIMEOUT;
      motorPower = power;
      this.targetPos = targetPos;
    }

    public PhiAction createAction_testJointRange() {
      return
        new PhiAction_Sequence(new PhiAction[] {
          new PhiAction_MoveWithTimeout(this, TEST_MOTOR_POWER, "findMaxRange"),
          new PhiAction_RunBlock(delegate() {
            // save max
            maxJointPos = PHI_currPos;
          }, "save_max"),
          new PhiAction_MoveWithTimeout(this, -TEST_MOTOR_POWER, "findMinRange"),
          new PhiAction_RunBlock(delegate() {
            minJointPos = PHI_currPos;
            centerJointPos = (minJointPos + maxJointPos) / 2;
            // debug
            Console.WriteLine("joint({0}): min={1}, max={2}, center={3}", motorIdx, minJointPos, maxJointPos, centerJointPos);
          }, "save_min"),
        });
    }

    // IPHICONTROLLER INTERFACE

    bool IPhiController.init(PhiStatePacket phiState, PhiCmdPacket phiCmds) {
      setIdle();
      return true;
    }

    void IPhiController.loadData(PhiStatePacket phiState) {
      PHI_currPos = phiState.getJointPos(motorIdx);
    }

    void IPhiController.step() {
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
          checkForMoveTimeout();
          break;

        case JOINT_STATE.SEEK_POS_WITH_TIMEOUT:
          if (PhiGlobals.approxEq(PHI_currPos, targetPos, JOINT_POS_TOLERANCE)) {
            // reached target - done
            setIdle();
          } else {
            // not there yet
            checkForMoveTimeout();
          }
          break;
      }
    }

    void checkForMoveTimeout() {
      UInt64 currTime = PhiGlobals.model.getPhiTime();
      if ((currTime - lastMoveTime) > MOVE_TIMEOUT_USEC) {
        if (PhiGlobals.approxEq(PHI_currPos, lastMovePos, JOINT_POS_TOLERANCE)) {
          // timeout has passed and joint did not move enough
          setTimedOut();
        } else {
          // moved enough - keep going
          lastMovePos = PHI_currPos;
          lastMoveTime = currTime;
        }
      }
    }

    void IPhiController.outputData(PhiCmdPacket phiCmds) {
      phiCmds.setMotorPower(motorIdx, motorPower);
    }

    void IPhiController.shutdown() {
    }

  } // PhiJoint class

  //
  // PHI ACTIONS
  //

  public class PhiAction_MoveWithTimeout : PhiAction {
    
    PhiJoint joint;
    
    public PhiAction_MoveWithTimeout(PhiJoint joint, double power, string name = "") : base(name) {

      this.joint = joint;
      codeBlock = delegate() {

        switch (actionState) {

          case ACTION_STATE.INIT:
            // set power
            joint.moveWithTimeout(power);
            actionState = ACTION_STATE.RUNNING;
            break;

          case ACTION_STATE.RUNNING:
            if (joint.isIdle()) {
              // done
              actionState = ACTION_STATE.DONE;
            }
            break;
        }
      };
    }

    public override void dump(string indentString = "") {
      dumpCommon(indentString, "MoveWithTimeout");
      Console.WriteLine();
    }
  } // PhiAction_MoveWithTimeout

} // namespace
