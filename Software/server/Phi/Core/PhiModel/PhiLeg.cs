
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  public class PhiLeg : IPhiController {

    //
    // CONSTANTS
    //

    public const int THIGH_IDX = 0;
    public const int SHIN_IDX  = 1;
    public const int HIP_IDX   = 2;

    //
    // VARS
    //

    public PhiJoint[] joints = new PhiJoint[3];

    //
    // CODE
    //

    public PhiLeg(int legIdx) {
      joints[THIGH_IDX] = new PhiJoint(legIdx * 2);
      joints[SHIN_IDX]  = new PhiJoint((legIdx * 2) + 1);
      joints[HIP_IDX]   = new PhiJoint(legIdx + 8);
    }

    public void reset() {
      // pass on to children
      foreach (PhiJoint joint in joints) {
        joint.reset();
      }
    }

    /*public PhiAction createAction_testLeg() {
      return new PhiAction_Sequence(new PhiAction[] {
        joints[PhiLeg.THIGH_IDX].createAction_testJointRange(),
        new PhiAction_RunBlock(delegate() {
          // save max
          maxJointPos = PHI_currPos;
        }),
      });
    }*/

    // IPHICONTROLLER INTERFACE

    bool IPhiController.init(PhiStatePacket phiState, PhiCmdPacket phiCmds) {
      // do model specific stuff
      bool rc = true;
      // pass on to children
      foreach (IPhiController joint in joints) {
        rc &= joint.init(phiState, phiCmds);
      }
      // done
      return true;
    }

    void IPhiController.loadData(PhiStatePacket phiState) {
      // pass on to children
      foreach (IPhiController joint in joints) {
        joint.loadData(phiState);
      }
    }

    void IPhiController.step() {
      // pass on to children
      foreach (IPhiController joint in joints) {
        joint.step();
      }
    }

    void IPhiController.outputData(PhiCmdPacket phiCmds) {
      // pass on to children
      foreach (IPhiController joint in joints) {
        joint.outputData(phiCmds);
      }
    }

    void IPhiController.shutdown() {
      // pass on to children
      foreach (IPhiController joint in joints) {
        joint.shutdown();
      }
    }
    
    //
    // ACTION FACTORY
    //

    public PhiAction createAction_calibrateLeg() {

      return
        new PhiAction_Sequence(name: "calibrate_leg", 
                               actions:  new PhiAction[] {

          new PhiAction_SeekWithTimeout(name: "clear_shin",
                                        joint: joints[PhiLeg.SHIN_IDX],
                                        targetPos: 0.9),

          joints[PhiLeg.THIGH_IDX].createAction_calibrateJoint(0.1),      // test range of thigh joint
          joints[PhiLeg.SHIN_IDX].createAction_calibrateJoint(0.5),       // test range of shin joint

      });
    }

    public PhiAction createAction_extendLegFlat() {
      return
        new PhiAction_Group(name: "extend_leg_flat",
                            actions: new PhiAction[] {

          new PhiAction_SeekWithTimeout(name: "extend_thigh", joint: joints[PhiLeg.THIGH_IDX], targetPos: 0.1),
          new PhiAction_SeekWithTimeout(name: "extend_shin",  joint: joints[PhiLeg.SHIN_IDX],  targetPos: 0.8),

      });
    }

  } // PhiLeg class

} // namesapce
