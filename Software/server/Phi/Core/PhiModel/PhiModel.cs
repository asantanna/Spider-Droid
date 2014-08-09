

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {
  class PhiModel : IPhiController {

    // CONSTANTS

    public const int LEG_A_IDX = 0;
    public const int LEG_B_IDX = 1;
    public const int LEG_C_IDX = 2;
    public const int LEG_D_IDX = 3;

    //
    // VARS
    //

    public PhiLeg[] legs = new PhiLeg[4];
    private PhiAction_Group rootAction;

    // from PHI state
    private UInt64 PHI_currTime;

    //
    // CODE
    //

    public PhiModel() {

      // allocate legs                              // joint[TSH] : joint_idx [0-11] : JSON motor name "[A-F][0-1]"
      legs[LEG_A_IDX] = new PhiLeg(LEG_A_IDX);      // motors T:0:"A0", S:1:"A1", H:8:"E0"
      legs[LEG_B_IDX] = new PhiLeg(LEG_B_IDX);      // motors T:2:"B0", S:3:"B1", H:9:"E1"
      legs[LEG_C_IDX] = new PhiLeg(LEG_C_IDX);      // motors T:4:"C0", S:5:"C1", H:10:"F0"
      legs[LEG_D_IDX] = new PhiLeg(LEG_D_IDX);      // motors T:6:"D0", S:7:"D1", H:11:"F1"

      // create the root action
      abortAllActions();
    }

    public void addChildAction(PhiAction action) {
      rootAction.addChild(action);
    }

    public UInt64 getPhiTime() {
      return PHI_currTime;
    }

    //
    // HACK TODO - we need to do some sort of lock for messing with actions! - HACK
    //

    public void abortAllActions() {
      // get rid of current hierarchy if it exists so
      // actions stop before start messing around
      rootAction = null;
      // reset action system
      PhiAction.resetActionFramework();
      // create a new root action
      rootAction = new PhiAction_Group(new PhiAction[0], repeatCount: PhiAction_Group.REPEAT_FOREVER);
      // reset the model
      reset();
    }

    public void abortAllActionsAndDump() {
      // save old hierarchy
      PhiAction_Group oldRoot = rootAction;
      // abort all actions
      abortAllActions();
      // dump old hierarchy to console
      dumpActionHierarchy(oldRoot);
      // DEBUG: force garbage collection
      oldRoot = null;
      System.GC.Collect();
    }

    public void dumpActionHierarchy(PhiAction_Group rootNode = null) {
      if (rootNode == null) {
        rootNode = rootAction;
      }
      Console.WriteLine("============================");
      Console.WriteLine("  PhiAction Hierarchy Dump");
      Console.WriteLine("============================");
      Console.WriteLine("dictionary size = {0}", PhiAction.getDictSize());
      rootNode.dump();
    }

    // Note: PhiModel.reset() is private because resetting the model isn't much help
    // if you don't abort all the actions first.  So we should always use the
    // abortAllActions*() functions instead.

    private void reset() {
      // pass on to children
      foreach (PhiLeg leg in legs) {
        leg.reset();
      }
      // DEBUG
      Console.WriteLine("*** Model was reset.");
    }

    //
    // IPHICONTROLLER
    //

    bool IPhiController.init(PhiStatePacket phiState, PhiCmdPacket phiCmds) {
      bool rc = true;
      // pass on to children
      foreach (IPhiController leg in legs) {
        rc &= leg.init(phiState, phiCmds);
      }
      // done
      return true;
    }

    void IPhiController.loadData(PhiStatePacket phiState) {
      PHI_currTime = phiState.getPhiTime_uSecs();
      // pass on to children
      foreach (IPhiController leg in legs) {
        leg.loadData(phiState);
      }
    }

    void IPhiController.step() {
      // execute all actions
      rootAction.step();
      // pass on to controller children
      foreach (IPhiController leg in legs) {
        leg.step();
      }
    }

    void IPhiController.outputData(PhiCmdPacket phiCmds) {
      // pass on to children
      foreach (IPhiController leg in legs) {
        leg.outputData(phiCmds);
      }
    }

    void IPhiController.shutdown() {
      // pass on to children
      foreach (IPhiController leg in legs) {
        leg.shutdown();
      }
    }

  } // PhiModel class
} // namespace
