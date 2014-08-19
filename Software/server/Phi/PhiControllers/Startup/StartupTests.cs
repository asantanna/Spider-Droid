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
        new PA_Sequence(name: "startup_tests",
                               repeatCount: bRepeatForever ? PA_Group.REPEAT_FOREVER : 1,
                               actions: new PhiActionBase[] {

          new PhiLeg.PA_Calibrate(PhiGlobals.model.legs[PhiModel.LEG_A_IDX]),
                        
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

    public void extendLegFlat(int legIdx) {
      // create an action to extend leg
      PhiLeg leg = PhiGlobals.model.legs[legIdx];
      PhiGlobals.model.addChildAction(leg.createAction_extendLegFlat());
    }

    public void centerJoints(int legIdx) {
      // create an action to extend leg
      PhiLeg leg = PhiGlobals.model.legs[legIdx];
      PhiGlobals.model.addChildAction(leg.createAction_centerJoints());
    }

    public void testLeg(int legIdx) {

      bool bRepeatForever = false;

      PhiGlobals.model.addChildAction(
        new PA_Sequence(name: "startup_tests",
                               repeatCount: bRepeatForever ? PA_Group.REPEAT_FOREVER : 1,
                               actions: new PhiActionBase[] {

          new PhiLeg.PA_TestLeg(PhiGlobals.model.legs[PhiModel.LEG_A_IDX]),

        })
      );

      // DEBUG
      // PhiGlobals.model.dumpActionHierarchy();
    }

  } // class StartupController()
} // namespace
