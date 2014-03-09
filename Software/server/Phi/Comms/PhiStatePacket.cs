using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi
{
  class PhiStatePacket : PhiBasePacket {

    //
    // CONST
    //

    // state packet fields
    
    private enum  STATE_FIELD_ENUM {
      SIGN          = 0,
      PACKET_ID     = 1,
      IMAGE         = 2,
      JOINT_POS     = 3,
      GYRO          = 4,
      ACCEL         = 5,
      TEMPERATURE   = 6,
    };

    //
    // VARS
    //

    // packet field definitions
    private static PACKET_FIELD_DEF[] fieldDefs = {
      new PACKET_FIELD_DEF("sign", 4),
      new PACKET_FIELD_DEF("packet_id", 4),
      new PACKET_FIELD_DEF("image", NUM_IMAGE_ELEM),
      new PACKET_FIELD_DEF("joint_pos", NUM_JOINT_POS_ELEM * sizeof(float)),
      new PACKET_FIELD_DEF("gyro", NUM_GYRO_ELEM * sizeof(float)),
      new PACKET_FIELD_DEF("accel", NUM_ACCEL_ELEM * sizeof(float)),
      new PACKET_FIELD_DEF("temperature", NUM_TEMP_ELEM * sizeof(float)), 
    };

    //
    // CODE
    //

    internal PhiStatePacket() : base(fieldDefs) {
      // base does everything
    }

    internal double getPitch() {
      double raw = (double)PhiGlobals.MAKE_FLOAT(packetData, fieldOffs[(int)STATE_FIELD_ENUM.GYRO]);
      return raw;
    }

    internal double getYaw() {
      double raw = (double)PhiGlobals.MAKE_FLOAT(packetData, fieldOffs[(int)STATE_FIELD_ENUM.GYRO] + 4);
      return raw;
    }

    internal double getRoll() {
      double raw = (double)PhiGlobals.MAKE_FLOAT(packetData, fieldOffs[(int)STATE_FIELD_ENUM.GYRO] + 8);
      return raw;
    }

    internal void getJointData(double[] joints) {
      for (int i = 0 ; i < NUM_MOTOR_ELEM ; i++) {
        joints[i] = (double)PhiGlobals.MAKE_FLOAT(packetData, fieldOffs[(int)STATE_FIELD_ENUM.JOINT_POS] + (i * sizeof(float)));
      }
    }

    internal UInt32 getPacketID() {
      UInt32 raw = PhiGlobals.MAKE_UINT32(packetData, fieldOffs[(int)STATE_FIELD_ENUM.PACKET_ID]);
      return raw;
    }

  } // class
} // namespace
