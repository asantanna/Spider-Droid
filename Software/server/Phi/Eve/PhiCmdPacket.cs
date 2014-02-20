using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  class PhiCmdPacket {

    //
    // CONST
    //

    private const int PACKET_SIGN_LEN = 4;
    private static byte[] CMD_PACKET_SIGN = PhiGlobals.StrToByteArray("CPV1");

    // offsets in packetData
    private const int OFF_SIGN = 0;
    private const int OFF_MOTOR_POWER = OFF_SIGN + PACKET_SIGN_LEN;
    private const int OFF_PACKET_END = OFF_MOTOR_POWER + PhiGlobals.NUM_MOTOR_ELEM;

    //
    // VARS
    //

    internal byte[] packetData = new byte[OFF_PACKET_END];

    internal int Length {
      get {
        return OFF_PACKET_END;
      }
    }

    internal PhiCmdPacket() {
      // init signature
      CMD_PACKET_SIGN.CopyTo(packetData, OFF_SIGN);
    }

  } // class
} // namespace

