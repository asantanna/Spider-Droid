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
        new PhiAction_Sequence(name: "startup_tests",
                               repeatCount: bRepeatForever ? PhiAction_Group.REPEAT_FOREVER : 1,
                               actions: new PhiAction[] {

          PhiGlobals.model.legs[PhiModel.LEG_A_IDX].createAction_calibrateLeg(),      // test range of leg A

          new PhiAction_RunBlock(testsComplete),                                      // call tests done function

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
      PhiGlobals.model.addChildAction( leg.createAction_extendLegFlat());
    }

  }
}
