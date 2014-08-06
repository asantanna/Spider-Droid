
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

  } // PhiLeg class

  //
  // PHI ACTIONS
  //

} // namesapce
