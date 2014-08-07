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
    public const UInt64 MOVE_TIMEOUT_USEC = 300 * 1000;

    // ADC has 1024 resolution mapped to [0,1] so delta is approx 0.001
    // Values closer than the tolerance below are considered "equal"
    public const double JOINT_POS_TOLERANCE = 0.01;

    // motor power used for startup tests
    public const double TEST_MOTOR_POWER = 0.2;  

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

    // DEBUG
    private double minSeekDelta;        // used for debugging tolerances

    //
    // CODE
    //

    public PhiJoint(int motorIdx) {
      this.motorIdx = motorIdx;
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
      minSeekDelta = 1.0;
    }

    void resetTimeout() {
      // set up to time out if we don't move for a while
      lastMovePos = PHI_currPos;
      lastMoveTime = PhiGlobals.model.getPhiTime();
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

        case JOINT_STATE.SEEK_POS_WITH_TIMEOUT: {
          double delta = PHI_currPos-targetPos;
          // DEBUG
          minSeekDelta = Math.Min(minSeekDelta, Math.Abs(delta));
          // check if done
          if (PhiGlobals.approxEq(PHI_currPos, targetPos, JOINT_POS_TOLERANCE)) {
            // reached target - done
            setIdle();
          } else {
            // not at target
            if ( ((motorPower > 0) && (delta > 0)) ||
                 ((motorPower < 0) && (delta < 0))) {
              // overshot target - done
              setIdle();
              // DEBUG
              Console.WriteLine("*** Overshoot occurred when seeking to joint position, minDelta={0:G3}", minSeekDelta);
            } else {
              // on way to target - check if moving
              double hack = motorPower;
              checkForMoveTimeout();
              // DEBUG
              if (jointState == JOINT_STATE.TIMEOUT_IDLE) {
                Console.WriteLine("*** Timeout occurred when seeking to joint position, minDelta={0:G3}", minSeekDelta);
              }
            }
          }
        }
        // DEBUG
        // Console.WriteLine("joint seek: curr={0:G3}, target={1:G3}, delta={2:G3}", 
        //                   PHI_currPos, targetPos, Math.Abs(PHI_currPos-targetPos));
        break;
      }
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

    void IPhiController.outputData(PhiCmdPacket phiCmds) {
      phiCmds.setMotorPower(motorIdx, motorPower);
    }

    void IPhiController.shutdown() {
    }

    //
    // ACTION FACTORY
    //

    public PhiAction createAction_calibrateJoint(double safePosFrac) {
      // init to shut up compiler
      PhiAction_Sequence seq = null;
      // create test, saving the sequence so children can be added by RunBlocks
      seq = new PhiAction_Sequence(name: "test_joint_range",
                                   actions:new PhiAction[] {

          new PhiAction_MoveWithTimeout(name: "find_max_range", joint: this, power: TEST_MOTOR_POWER),

          new PhiAction_RunBlock(name: "save_max", code: delegate() {
            // save max
            maxJointPos = PHI_currPos;
          }),

          new PhiAction_MoveWithTimeout(name: "find_min_range", joint: this, power: -TEST_MOTOR_POWER),

          new PhiAction_RunBlock(name: "save_min_and_park", code: delegate() {
            // save min and center
            minJointPos = PHI_currPos;
            centerJointPos = (minJointPos + maxJointPos) / 2;

            // DEBUG
            Console.WriteLine("joint({0:G3}): min={1:G3}, max={2:G3}, center={3:G3}",
                               motorIdx, minJointPos, maxJointPos, centerJointPos);

            // now that we know the center, create an action to move to a safe position relative to the span
            double safePos = minJointPos + (maxJointPos - minJointPos) * safePosFrac;

            PhiAction seekAction = new PhiAction_SeekWithTimeout(name: "seek_safe", joint: this,
                                                                 absPower: TEST_MOTOR_POWER, targetPos: safePosFrac);

            // mark action as auto remove since every time this sequence is repeated, a new one gets created
            seekAction.setFlagBits(PhiAction.ACTION_FLAGS.AUTO_REMOVE);

            // add to our parent sequence
            seq.addChild(seekAction);

            // DEBUG
            // PhiGlobals.model.dumpActionHierarchy();
          }),

        });

      return seq;
    }

  } // PhiJoint class

  //
  // PHI ACTIONS
  //

  // MOVE WITH TIMEOUT ACTION

  public class PhiAction_MoveWithTimeout : PhiAction {

    PhiJoint joint;
    double reqPower;

    public PhiAction_MoveWithTimeout(PhiJoint joint, double power, string name = "") : base(name) {

      this.joint = joint;
      // save these because we will need them so dump() can report them
      reqPower = power;

      codeBlock = delegate() {
        switch (actionState) {
          case ACTION_STATE.INIT:
            // set power
            joint.moveWithTimeout(reqPower);
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
      Console.WriteLine(", power={0:G2}", reqPower);
    }
  } // PhiAction_MoveWithTimeout

  // SEEK WITH TIMEOUT ACTION

  public class PhiAction_SeekWithTimeout : PhiAction {

    PhiJoint joint;
    double reqAbsPower;
    double reqTargetPos;

    public PhiAction_SeekWithTimeout(
              PhiJoint joint, 
              double absPower = PhiJoint.TEST_MOTOR_POWER,
              double targetPos = 0.5,
              string name = "") : base(name) {

      this.joint = joint;
      // save these because we will need them so dump() can report them
      reqAbsPower = Math.Abs(absPower);
      reqTargetPos = targetPos;

      codeBlock = delegate() {
        switch (actionState) {
          case ACTION_STATE.INIT: {
            // figure out which way to move
            double power = (joint.getPos() <= targetPos) ? reqAbsPower : -reqAbsPower;
            // set power
            joint.seekWithTimeout(power, reqTargetPos);
            actionState = ACTION_STATE.RUNNING;
          }
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
      dumpCommon(indentString, "SeekWithTimeout");
      Console.WriteLine(", absPower={0:G2}, target={1:G4}", reqAbsPower, reqTargetPos);
    }
  } // PhiAction_SeekWithTimeout

} // namespace
