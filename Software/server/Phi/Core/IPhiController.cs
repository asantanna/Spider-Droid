using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {
  public interface IPhiController {
    // initialize controller
    bool init(PhiStatePacket phiState, PhiCmdPacket phiCmds);
    // load data from PHI state packet into controller
    void loadData(PhiStatePacket phiState);
    // perform a single step
    void step();
    // output data from controller to PHI command packet
    void outputData(PhiCmdPacket phiCmds);
    // shut down controller
    void shutdown();
  }
}
