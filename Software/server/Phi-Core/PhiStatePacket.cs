using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi_Core
{
  class PhiStatePacket {
    //
    // CONST
    //

    private const int PACKET_SIGN_LEN = 4;
    private static byte[] STATE_PACKET_SIGN = PhiGlobals.StrToByteArray("STAP");

    // offsets in packetData
    private const int OFF_SIGN = 0;
    private const int OFF_VISION_AXONS = OFF_SIGN + PACKET_SIGN_LEN;
    private const int OFF_GYRO_AXONS = OFF_VISION_AXONS + PhiGlobals.NUM_VISION_AXONS;
    private const int OFF_JOINT_POS_AXONS = OFF_GYRO_AXONS + PhiGlobals.NUM_GYRO_AXONS;
    private const int OFF_TEMP_AXONS = OFF_JOINT_POS_AXONS + PhiGlobals.NUM_JOINT_POS_AXONS;
    private const int OFF_PACKET_END = OFF_TEMP_AXONS + PhiGlobals.NUM_TEMP_AXONS;

    //
    // VARS
    //

    internal byte[] packetData = new byte[OFF_PACKET_END];

    internal int Length {
      get {
        return OFF_PACKET_END;
      }
    }

    internal PhiStatePacket() {
      // nothing to do
    }
  }
}
