
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  public partial class PhiLeg : IPhiController {

    //
    // CONSTANTS
    //

    public const int THIGH_IDX = 0;
    public const int SHIN_IDX  = 1;
    public const int HIP_IDX   = 2;

    //
    // VARS
    //

    private string name;
    public PhiJoint[] joints = new PhiJoint[3];

    //
    // CODE
    //

    public PhiLeg(string name, int legIdx) {
      this.name = name;
      joints[THIGH_IDX] = new PhiJoint(name + ".T", legIdx * 2);
      joints[SHIN_IDX]  = new PhiJoint(name + ".S", (legIdx * 2) + 1);
      joints[HIP_IDX]   = new PhiJoint(name + ".H", legIdx + 8);
    }

    public void reset() {
      // pass on to children
      foreach (PhiJoint joint in joints) {
        joint.reset();
      }
    }

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

} // namesapce
