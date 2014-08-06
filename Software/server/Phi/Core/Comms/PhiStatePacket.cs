using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi
{
  public class PhiStatePacket : PhiBasePacket {

    //
    // CONST
    //

    // state packet fields
    
    private enum  STATE_FIELD_ENUM {
      SIGN          = 0,
      PACKET_ID     = 1,
      PHI_TIME      = 2,
      IMAGE         = 3,
      JOINT_POS     = 4,
      GYRO          = 5,
      ACCEL         = 6,
      TEMPERATURE   = 7,
    };

    //
    // VARS
    //

    // packet field definitions
    private static PACKET_FIELD_DEF[] fieldDefs = {
      new PACKET_FIELD_DEF("sign", 4),
      new PACKET_FIELD_DEF("packet_id", 4),
      new PACKET_FIELD_DEF("phi_time", 8),                                        // up time uSecs - UINT64
      new PACKET_FIELD_DEF("image", NUM_IMAGE_ELEM),                              // camera image - format TBD
      new PACKET_FIELD_DEF("joint_pos", NUM_JOINT_POS_ELEM * sizeof(float)),      // position - range: [0, 1]
      new PACKET_FIELD_DEF("gyro", NUM_GYRO_ELEM * sizeof(float)),                // change - range: [-1, 1] means [-360, 360] degrees
      new PACKET_FIELD_DEF("accel", NUM_ACCEL_ELEM * sizeof(float)),              // accel - range: TBD
      new PACKET_FIELD_DEF("temperature", NUM_TEMP_ELEM * sizeof(float)),         // temperature - range: [0, 1] means [-40, 85] C
    };

    //
    // CODE
    //

    public PhiStatePacket() : base(fieldDefs) {
      // base does everything
    }

    public double getPitchDelta() {
      double raw = (double) PhiGlobals.READ_FLOAT(packetData, fieldOffs[(int)STATE_FIELD_ENUM.GYRO]);
      return raw;
    }

    public double getYawDelta() {
      double raw = (double) PhiGlobals.READ_FLOAT(packetData, fieldOffs[(int)STATE_FIELD_ENUM.GYRO] + 4);
      return raw;
    }

    public double getRollDelta() {
      double raw = (double) PhiGlobals.READ_FLOAT(packetData, fieldOffs[(int)STATE_FIELD_ENUM.GYRO] + 8);
      return raw;
    }

    public double getJointPos(int idx) {
      return (double) PhiGlobals.READ_FLOAT(packetData, fieldOffs[(int)STATE_FIELD_ENUM.JOINT_POS] + (idx * sizeof(float)));
    }

    public UInt32 getPacketID() {
      return PhiGlobals.READ_UINT32(packetData, fieldOffs[(int)STATE_FIELD_ENUM.PACKET_ID]);
    }

    public UInt64 getPhiTime_uSecs() {
      return PhiGlobals.READ_UINT64(packetData, fieldOffs[(int)STATE_FIELD_ENUM.PHI_TIME]);
    }

  } // class
} // namespace
