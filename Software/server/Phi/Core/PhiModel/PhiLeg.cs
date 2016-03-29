
using Phi.UI;
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

    // log tree view node name (key)
    public const string LOG_NODENAME_LEGS_PREFIX = "logNode_Leg_";

    //
    // VARS
    //

    private PhiModel parent;
    public string name;

    public PhiJoint[] joints = new PhiJoint[3];

    // log tree view node
    public System.Windows.Forms.TreeNode logTreeNode;

    //
    // CODE
    //

    public PhiLeg(PhiModel parent, string name, int legIdx) {
      this.parent = parent;
      this.name = name;

      // create log node
      logTreeNode = new System.Windows.Forms.TreeNode(LOG_NODENAME_LEGS_PREFIX + name);
      logTreeNode.Tag = null;
      logTreeNode.Text = "Leg " + name;
      parent.logTreeNode.Nodes.Add(logTreeNode);

      // create joints
      joints[THIGH_IDX] = new PhiJoint(this, "T", legIdx * 2);
      joints[SHIN_IDX]  = new PhiJoint(this, "S", (legIdx * 2) + 1);
      joints[HIP_IDX]   = new PhiJoint(this, "H", legIdx + 8);
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
