using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  partial class StartupController : IPhiController {

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

  } // class
} // namespace
