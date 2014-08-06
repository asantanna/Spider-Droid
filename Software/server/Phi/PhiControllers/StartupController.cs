using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  class StartupController : IPhiController {

    //
    // VARS
    //

    //
    // CODE
    //

    public StartupController() {
    }

    //
    // IPHICONTROLLER INTERFACE
    //

    bool IPhiController.init(PhiStatePacket phiState, PhiCmdPacket phiCmds) {
      // debug
      Debug.WriteLine("PhiStartup: Init");

      // pass on to model
      (PhiGlobals.model as IPhiController).init(phiState, phiCmds);

      // success
      return true;
    }

    void IPhiController.loadData(PhiStatePacket phiState) {
      // pass on to model
      // Note: model stores contents of phiState in its classes
      (PhiGlobals.model as IPhiController).loadData(phiState);
    }

    void IPhiController.step() {
      // pass on to model
      // Note: model performs active tasks here
      (PhiGlobals.model as IPhiController).step();
    }

    void IPhiController.outputData(PhiCmdPacket phiCmds) {
      // pass on to model
      // Note: data is filled in by the model's classes
      (PhiGlobals.model as IPhiController).outputData(phiCmds);
    }

    void IPhiController.shutdown() {
      // pass on to model
      (PhiGlobals.model as IPhiController).shutdown();
    }

    //
    // INITIALIZATION TESTS
    //

    public void startTests() {
      Console.WriteLine("Starting tests ...");
      // test range of leg A
      PhiGlobals.model.addChildAction(
        new PhiAction_Sequence(new PhiAction[]
        {
          PhiGlobals.model.legs[PhiModel.LEG_A_IDX].joints[PhiLeg.THIGH_IDX].createAction_testJointRange(),
          new PhiAction_RunBlock(testsComplete),
        }
        ));

      // DEBUG
      PhiGlobals.model.dumpActionHierarchy();
    }

    public static void testsComplete() {
      Console.WriteLine("Startup tests complete");
      // switch to eve controller or something
    }

  } // class
} // namespace
