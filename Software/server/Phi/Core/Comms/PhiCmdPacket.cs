using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  public class PhiCmdPacket : PhiBasePacket {

    //
    // CONST
    //

    // cmd packet fields

    private enum CMD_FIELD_ENUM {
      SIGN          = 0,
      PACKET_ID     = 1,
      MOTOR_CMDS    = 2,
    };

    // cmd packet field definitions
    private static PACKET_FIELD_DEF[] fieldDefs = {
      new PACKET_FIELD_DEF("sign", 4),
      new PACKET_FIELD_DEF("packet_id", 4),
      new PACKET_FIELD_DEF("motor_cmds", NUM_MOTOR_ELEM * sizeof(float)),       // range: [-1, 1]
    };

    //
    // VARS
    //

    private static byte[] CMD_PACKET_SIGN = PhiGlobals.StrToByteArray("CPV1");

    public PhiCmdPacket() : base(fieldDefs) {
      // init signature
      CMD_PACKET_SIGN.CopyTo(packetData, fieldOffs[(int)CMD_FIELD_ENUM.SIGN]);
    }

    public void setMotorPower(int idx, double power) {
      PhiGlobals.WRITE_FLOAT((float) power, packetData, fieldOffs[(int)CMD_FIELD_ENUM.MOTOR_CMDS] + (idx * sizeof(float)));
    }

  } // class
} // namespace

