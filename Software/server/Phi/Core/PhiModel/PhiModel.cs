

using Phi.UI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {
  public class PhiModel : IPhiController {

    // CONSTANTS

    public const int LEG_A_IDX = 0;
    public const int LEG_B_IDX = 1;
    public const int LEG_C_IDX = 2;
    public const int LEG_D_IDX = 3;

    // log tree view node name (key)
    public const string LOG_NODENAME_MODEL = "logNode_Model";

    //
    // VARS
    //

    public PhiLeg[] legs = new PhiLeg[4];

    // root of action hierarchy
    private PA_Group rootAction;

    // log tree view node
    public System.Windows.Forms.TreeNode logTreeNode;

    // from PHI state
    public UInt64 PHI_currTime;

    //
    // CODE
    //

    public PhiModel() {

      // create log node
      logTreeNode = new System.Windows.Forms.TreeNode(LOG_NODENAME_MODEL);
      logTreeNode.Tag = null;
      logTreeNode.Text = "Model";

      // This should not use "LogForm.invokeAddNode" because the log window might not be created yet.
      // Note: this is OK because we are running on the UI thread anyway
      PhiGlobals.logForm.logTreeRootNode.Nodes.Add(logTreeNode);

      // allocate legs                                         // joint[TSH] : joint_idx [0-11] : JSON motor name "[A-F][0-1]"
      legs[LEG_A_IDX] = new PhiLeg(this, "A", LEG_A_IDX);      // motors T:0:"A0", S:1:"A1", H:8:"E0"
      legs[LEG_B_IDX] = new PhiLeg(this, "B", LEG_B_IDX);      // motors T:2:"B0", S:3:"B1", H:9:"E1"
      legs[LEG_C_IDX] = new PhiLeg(this, "C", LEG_C_IDX);      // motors T:4:"C0", S:5:"C1", H:10:"F0"
      legs[LEG_D_IDX] = new PhiLeg(this, "D", LEG_D_IDX);      // motors T:6:"D0", S:7:"D1", H:11:"F1"

      // show all log nodes
      PhiGlobals.logForm.ExpandAll();

      // create the root action
      abortAllActions();
    }

    public void addChildAction(PhiActionBase action) {
      rootAction.addChild(action);
    }

    //
    // HACK TODO - we need to do some sort of lock for messing with actions! - HACK
    //

    public void abortAllActions() {
      // get rid of current hierarchy if it exists so
      // actions stop before start messing around
      rootAction = null;
      // reset action system
      PhiActionBase.resetActionFramework();
      // create a new root action
      rootAction = new PA_Group(new PhiActionBase[0], repeatCount: PA_Group.REPEAT_FOREVER);
      // reset the model
      reset();
    }

    public void abortAllActionsAndDump() {
      // save old hierarchy
      PA_Group oldRoot = rootAction;
      // abort all actions
      abortAllActions();
      // dump old hierarchy to console
      dumpActionHierarchy(oldRoot);
      // DEBUG: force garbage collection
      oldRoot = null;
      System.GC.Collect();
    }

    public void dumpActionHierarchy(PA_Group rootNode = null) {
      if (rootNode == null) {
        rootNode = rootAction;
      }
      Console.WriteLine("============================");
      Console.WriteLine("  PhiAction Hierarchy Dump");
      Console.WriteLine("============================");
      Console.WriteLine("dictionary size = {0}", PhiActionBase.getDictSize());
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
