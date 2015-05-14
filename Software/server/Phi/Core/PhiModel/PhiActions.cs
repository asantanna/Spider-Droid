using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  //
  // MODEL ACTIONS
  //

  //
  // LEG ACTIONS
  //
  
  public partial class PhiLeg {
    
    // CALIBRATE ACTION

    public class PA_Calibrate : PA_Sequence {

      public PA_Calibrate(PhiLeg leg)
        : base(name: "calibrate_leg",
               actions: new PhiActionBase[] {

          new PhiJoint.PA_SeekWithTimeout(name: "clear_shin",
                                                 joint: leg.joints[PhiLeg.SHIN_IDX],
                                                 targetPos: 0.9),

          leg.joints[PhiLeg.THIGH_IDX].createAction_calibrateJoint(0.1),      // test range of thigh joint
          leg.joints[PhiLeg.SHIN_IDX].createAction_calibrateJoint(0.5),       // test range of shin joint

      }) { }
    } // class Calibrate

    // EXTEND LEG FULLY ACTION

    public PhiActionBase createAction_extendLegFully(int repeatCount = 1) {
      return
        new PA_Group(name: "extend_leg_flat",
                            repeatCount: repeatCount,
                            actions: new PhiActionBase[] {

          new PhiJoint.PA_SeekWithTimeout(name: "extend_thigh", joint: joints[PhiLeg.THIGH_IDX], targetPos: 1),
          new PhiJoint.PA_SeekWithTimeout(name: "extend_shin",  joint: joints[PhiLeg.SHIN_IDX],  targetPos: 1),

      });
    }

    // CENTER JOINTS ACTION

    public PhiActionBase createAction_centerJoints(int repeatCount = 1) {
      return
        new PA_Group(name: "center_joints",
                            repeatCount: repeatCount,
                            actions: new PhiActionBase[] {

          new PhiJoint.PA_SeekWithTimeout(name: "center_thigh", joint: joints[PhiLeg.THIGH_IDX], targetPos: 0.5),
          new PhiJoint.PA_SeekWithTimeout(name: "center_shin",  joint: joints[PhiLeg.SHIN_IDX],  targetPos: 0.5),

      });
    }

    // EXTEND LEG FULLY ACTION

    public PhiActionBase createAction_retractLegFully(int repeatCount = 1) {
      return
        new PA_Group(name: "retract_leg",
                            repeatCount: repeatCount,
                            actions: new PhiActionBase[] {

          new PhiJoint.PA_SeekWithTimeout(name: "retract_thigh", joint: joints[PhiLeg.THIGH_IDX], targetPos: 0),
          new PhiJoint.PA_SeekWithTimeout(name: "retract_shin",  joint: joints[PhiLeg.SHIN_IDX],  targetPos: 0),

      });
    }


    // TEST LEG ACTION

    public class PA_TestLeg : PA_Sequence {

      const int TEST_MS_1 = 750;

      public PA_TestLeg(PhiLeg leg, int repeatCount = 1)
        : base(name: "test_leg",
               repeatCount: repeatCount,
               actions: new PhiActionBase[] {

          new PhiJoint.PA_SeekWithTimeout(name: "clear_shin",
                                                 joint: leg.joints[PhiLeg.SHIN_IDX],
                                                 targetPos: 0.9,
                                                 absPower: PhiGlobals.testMotorPower),

          new PhiJoint.PA_MoveWithTimeout(name: "extend_thigh",
                                          joint: leg.joints[PhiLeg.THIGH_IDX],
                                          power: PhiGlobals.testMotorPower),

          // duration TEST_MS_1

          new PhiJoint.PA_AdaptiveSeekWithTimeout(name: "test_adapt_seek_t1",
                                                  joint: leg.joints[PhiLeg.THIGH_IDX],
                                                  targetPos: 0.2,
                                                  maxAbsPower: PhiGlobals.testMotorPower,
                                                  msDuration: TEST_MS_1),

          new PhiJoint.PA_AdaptiveSeekWithTimeout(name: "test_adapt_seek_t2",
                                                  joint: leg.joints[PhiLeg.THIGH_IDX],
                                                  targetPos: 0.8,
                                                  maxAbsPower: PhiGlobals.testMotorPower,
                                                  msDuration:  TEST_MS_1),

          new PhiJoint.PA_AdaptiveSeekWithTimeout(name: "test_adapt_seek_t3",
                                                  joint: leg.joints[PhiLeg.THIGH_IDX],
                                                  targetPos: 0.5,
                                                  maxAbsPower: PhiGlobals.testMotorPower,
                                                  msDuration:  TEST_MS_1),

          new PhiJoint.PA_AdaptiveSeekWithTimeout(name: "test_adapt_seek_s1",
                                                  joint: leg.joints[PhiLeg.SHIN_IDX],
                                                  targetPos: 0.1,
                                                  maxAbsPower: PhiGlobals.testMotorPower,
                                                  msDuration: TEST_MS_1),

          new PhiJoint.PA_AdaptiveSeekWithTimeout(name: "test_adapt_seek_s2",
                                                  joint: leg.joints[PhiLeg.SHIN_IDX],
                                                  targetPos: 0.9,
                                                  maxAbsPower: PhiGlobals.testMotorPower,
                                                  msDuration:  TEST_MS_1),

      }) { }
    } // class Calibrate

  } // class PhiLeg

  //
  // JOINT ACTIONS
  //

  public partial class PhiJoint {

    // CALIbRATE ACTION

    public PhiActionBase createAction_calibrateJoint(double safePosFrac) {

      // init to shut up compiler
      PA_Sequence seq = null;

      // create test, saving the sequence so children can be added by RunBlocks
      seq = new PA_Sequence(name: "test_joint_range",
                                   actions: new PhiActionBase[] {

          new PA_MoveWithTimeout(name: "find_max_range", joint: this, power: PhiGlobals.testMotorPower),

          new PA_RunBlock(name: "save_max", code: delegate() {
            // save max
            maxJointPos = PHI_currPos;
          }),

          new PA_MoveWithTimeout(name: "find_min_range", joint: this, power: -PhiGlobals.testMotorPower),

          new PA_RunBlock(name: "save_min_and_park", code: delegate() {
            // save min and center
            minJointPos = PHI_currPos;
            centerJointPos = (minJointPos + maxJointPos) / 2;

            // DEBUG
            Console.WriteLine("joint({0:G3}): min={1:G3}, max={2:G3}, center={3:G3}",
                               motorIdx, minJointPos, maxJointPos, centerJointPos);

            // now that we know the center, create an action to move to a safe position relative to the span
            double safePos = minJointPos + (maxJointPos - minJointPos) * safePosFrac;

            PhiActionBase seekAction = new PA_SeekWithTimeout(name: "seek_safe", joint: this,
                                                              absPower: PhiGlobals.testMotorPower, targetPos: safePosFrac);

            // mark action as auto remove since every time this sequence is repeated, a new one gets created
            seekAction.setFlagBits(PhiActionBase.ACTION_FLAGS.AUTO_REMOVE);

            // add to our parent sequence
            seq.addChild(seekAction);

            // DEBUG
            // PhiGlobals.model.dumpActionHierarchy();
          }),

        });

      return seq;
    }

    // MOVE WITH TIMEOUT ACTION

    public class PA_MoveWithTimeout : PhiActionBase {

      PhiJoint joint;

      public PA_MoveWithTimeout(PhiJoint joint, double power, string name = "")
        : base(name) {

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
        Console.WriteLine(", power={0:G2}", joint.getPower());
      }

    } // class PA_MoveWithTimeout

    // SEEK WITH TIMEOUT ACTION

    public class PA_SeekWithTimeout : PhiActionBase {

      PhiJoint joint;
      double req_absPower;

      public PA_SeekWithTimeout(
                PhiJoint joint,
                double targetPos,
                double absPower = PhiGlobals.DEF_TEST_MOTOR_POWER,
                string name = "")
        : base(name) {

        this.joint = joint;
        // save this for dump()
        req_absPower = Math.Abs(absPower);

        codeBlock = delegate() {
          switch (actionState) {
            case ACTION_STATE.INIT: {
                // figure out which way to move
                double power = (joint.getPos() <= targetPos) ? req_absPower : -req_absPower;
                // set power
                joint.seekWithTimeout(power, targetPos);
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
        Console.WriteLine(", absPower={0:G2}, target={1:G4}", req_absPower, joint.getTargetPos());
      }

    } // class PA_SeekWithTimeout

    // ADAPTIVE SEEK WITH TIMEOUT ACTION

    public class PA_AdaptiveSeekWithTimeout : PhiActionBase {

      //
      // CONSTS
      //

      // Motor power changes use these multipliers for increasing/decreasing the motor power.
      // Note: This scheme does not allow power to reverse to cause fast breaking.
      // Note: Multipliers ultimately have to be tweaked experimentally for best performance.
      // Note: Accel and decel multipliers do not need to be inverses.

      const double ACCEL_MULT = 1.1;
      const double DECEL_MULT = 1 / ACCEL_MULT;

      // Since power is increased by percentage, we cannot start at zero or it would never increase
      // Note: determined experimentally

      const double ABS_START_POWER = 0.1;

      //
      // VARS
      //

      PhiJoint joint;
      double req_msDuration;

      //
      // CODE
      //

      public PA_AdaptiveSeekWithTimeout(
                PhiJoint joint,
                double targetPos,
                double maxAbsPower = PhiGlobals.DEF_TEST_MOTOR_POWER,
                double msDuration = 0,
                string name = "")
        : base(name) {

        this.joint = joint;
        // save this for dump()
        req_msDuration = msDuration;

        codeBlock = delegate() {
          switch (actionState) {
            case ACTION_STATE.INIT: {
                // figure out which way to move
                double power = (joint.getPos() <= targetPos) ? ABS_START_POWER : -ABS_START_POWER;
                // set power
                joint.adaptiveSeekWithTimeout(targetPos: targetPos, maxAbsPower: maxAbsPower, msDuration: msDuration);    
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

        Console.WriteLine(", maxAbsPower={0:G2}, target={1:G4}, duration=[req={2:G3} mS, actual={3:G3} mS]",
                          joint.getMaxAbsPower(), joint.getTargetPos(), req_msDuration, joint.getDuration());
      }

    } // class PA_SeekWithTimeout

  } // class PhiJoint

  //
  // PRIMITIVE ACTIONS
  //

  // GROUP ACTION CLASS

  public class PA_Group : PhiActionBase {

    public const int REPEAT_FOREVER = -1;
    int repeatCount;
    protected int execCount;

    protected List<PhiActionBase> children = new List<PhiActionBase>();

    public PA_Group(PhiActionBase[] actions, string name = "", int repeatCount = 1)
      : base(name) {
      this.repeatCount = repeatCount;
      codeBlock = groupCodeBlock;
      // add children to list
      foreach (PhiActionBase action in actions) {
        addChild(action);
      }
    }

    void groupCodeBlock() {

      if (actionState == ACTION_STATE.INIT) {
        execCount = 0;
        actionState = ACTION_STATE.RUNNING;
      }

      if (actionState == ACTION_STATE.RUNNING) {

        // make a copy for iteration in case things get added/removed by children
        List<PhiActionBase> childrenCopy = children.ToList();

        // step all children
        // Note: they are stepped even if DONE - this is simpler and possibly faster than testing
        foreach (PhiActionBase child in childrenCopy) {
          child.step();
        }

        // now iterate again through real list to see if there's anybody left that's not DONE

        foreach (PhiActionBase child in children) {
          if (child.getState() != ACTION_STATE.DONE) {
            // found a child that's not DONE - means group isn't DONE
            return;
          }
        }

        // if we get here, all children are DONE so group is also
        notifyDone();
      }
    }

    // notifyDone(): called when all children of a group are done
    //
    // Note: when repeating, all children are reset when ALL of them are done.
    // For sequences, this is simple.  For groups, this imples an added subtlety:
    // even though group actions run concurrently, the repeat only occurs when all
    // of them are done.  So they "synchronize their restarts".

    protected void notifyDone() {
      execCount++;
      if ((repeatCount == REPEAT_FOREVER) || (execCount < repeatCount)) {
        // group will repeat - reset all actions in our tree to INIT state so they run again
        reset();
      } else {
        // group will not repeat on its own - mark it DONE
        actionState = ACTION_STATE.DONE;

        if (parentId == ROOT_ACTION_ID) {
          // our parent is the root so we are NOT part of a bigger hierarchy
          // that might get restarted due to an enclosing repeating group.
          // Go ahead and remove us completely.
          removeFromParent();
        }
      }
    }

    override public void addChild(PhiActionBase child) {
      child.setParentId(id);
      children.Add(child);
    }

    public void removeChild(PhiActionBase child) {
      children.Remove(child);
      child.discard();
    }

    public override void discard() {
      base.discard();
      // make list copy for iteration
      List<PhiActionBase> childrenCopy = children.ToList();
      foreach (PhiActionBase child in childrenCopy) {
        removeChild(child);
      }
    }

    public override void reset() {
      // reset group
      base.reset();
      // reset children
      foreach (PhiActionBase child in children) {
        child.reset();
      }
    }

    protected virtual void dumpGroup(string indentString, string header) {
      dumpCommon(indentString, header);
      Console.WriteLine(", children: {0}", children.Count);
      indentString += "| ";
      foreach (PhiActionBase child in children) {
        child.dump(indentString);
      }
    }

    public override void dump(string indentString = "") {
      dumpGroup(indentString, "Group");
    }

  } // PA_Group class

  // SEQUENCE ACTION

  public class PA_Sequence : PA_Group {

    int currChildIdx;

    public PA_Sequence(PhiActionBase[] actions, string name = "", int repeatCount = 1)
      : base(actions, name, repeatCount) {
      codeBlock = sequenceCodeBlock;
      // children already added by group ctor
    }

    void sequenceCodeBlock() {

      if (actionState == ACTION_STATE.INIT) {
        // SEQUENCE not started
        execCount = 0;
        currChildIdx = 0;
        actionState = ACTION_STATE.RUNNING;
      }

      if (actionState == ACTION_STATE.RUNNING) {
        // SEQUENCE running
        if (children.Count() > currChildIdx) {
          // child avail - step current child
          PhiActionBase child = children.ElementAt(currChildIdx);
          child.step();
          // check if child finished
          if (child.getState() == PhiActionBase.ACTION_STATE.DONE) {
            // child finished - advance to next child
            currChildIdx++;
          }
        } else {
          // no more children - say SEQUENCE is done
          notifyDone();
        }
      }
    }

    public override void dump(string indentString = "") {
      dumpGroup(indentString, "Sequence");
    }

  } // PA_Sequence class

  // RUNBLOCK ACTION

  public class PA_RunBlock : PhiActionBase {

    PhiActionCodeBlock userCodeBlock;

    public PA_RunBlock(PhiActionCodeBlock code, string name = "")
      : base(name) {

      // save for dump()
      userCodeBlock = code;

      codeBlock = delegate() {
        if (actionState == ACTION_STATE.INIT) {
          // run block
          actionState = ACTION_STATE.RUNNING;
          // call user code
          userCodeBlock();
          // action done
          actionState = ACTION_STATE.DONE;
        }
      };
    }

    public override void dump(string indentString = "") {
      dumpCommon(indentString, "RunBlock");
      Console.WriteLine(", codeBlock = {0}", userCodeBlock.Method.ToString());
    }

  } // PA_RunBlock

  // WAIT ACTION

  public class PA_Wait : PhiActionBase {
    UInt64 wakeup_uSecs;

    public PA_Wait(UInt64 mSecs) {

      codeBlock = delegate() {

        switch (actionState) {
          case ACTION_STATE.INIT: {
              // save wake time
              wakeup_uSecs = PhiGlobals.model.PHI_currTime + (mSecs * 1000);
              actionState = ACTION_STATE.RUNNING;
            }
            break;

          case ACTION_STATE.RUNNING: {
              if (wakeup_uSecs >= PhiGlobals.model.PHI_currTime) {
                // time to make up
                actionState = ACTION_STATE.DONE;
              }
            }
            break;
        }
      };
    }

    public override void dump(string indentString = "") {
      dumpCommon(indentString, "Wait");
      Console.WriteLine();
    }

  } // PA_Wait class

}
