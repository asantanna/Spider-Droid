using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

//
// INITIALIZATION TESTS
//

namespace Phi {

  partial class StartupController : IPhiController {

    public void startTests(bool bRepeatForever = false) {

      Console.WriteLine("Starting tests ...");

      // create an action for the startup tests

      PhiGlobals.model.addChildAction(
        new PA_Group(name: "startup_tests",
                     repeatCount: bRepeatForever ? PA_Group.REPEAT_FOREVER : 1,
                     actions: new PhiActionBase[] {

          new PhiLeg.PA_Calibrate(PhiGlobals.model.legs[PhiModel.LEG_A_IDX]),
          new PhiLeg.PA_Calibrate(PhiGlobals.model.legs[PhiModel.LEG_B_IDX]),
          new PhiLeg.PA_Calibrate(PhiGlobals.model.legs[PhiModel.LEG_C_IDX]),
          new PhiLeg.PA_Calibrate(PhiGlobals.model.legs[PhiModel.LEG_D_IDX]),
                        
          new PA_RunBlock(testsComplete),                                      // call tests done function

        })
      );

      // DEBUG
      // PhiGlobals.model.dumpActionHierarchy();
    }

    public void testsComplete() {
      Console.WriteLine("Startup tests complete");
      // switch to eve controller or something?
    }

    public void extendLegFull(int legIdx, int repeatCount) {
      // create an action to extend leg
      PhiLeg leg = PhiGlobals.model.legs[legIdx];
      PhiGlobals.model.addChildAction(leg.createAction_extendLegFully(repeatCount));
    }

    public void centerJoints(int legIdx, int repeatCount) {
      // create an action to extend leg
      PhiLeg leg = PhiGlobals.model.legs[legIdx];
      PhiGlobals.model.addChildAction(leg.createAction_centerJoints(repeatCount));
    }
    
    public void testLeg(int legIdx, int repeatCount) {

      PhiGlobals.model.addChildAction(
        new PA_Sequence(name: "leg_tests",
                               repeatCount: repeatCount,
                               actions: new PhiActionBase[] {

          new PhiLeg.PA_TestLeg(PhiGlobals.model.legs[legIdx]),

        })
      );

      // DEBUG
      // PhiGlobals.model.dumpActionHierarchy();
    }

  } // class StartupController()
} // namespace
