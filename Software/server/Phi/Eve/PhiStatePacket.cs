using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi
{
  class PhiStatePacket {

    //
    // CONST
    //

    private const int PACKET_SIGN_LEN = 4;
    private static byte[] STATE_PACKET_SIGN = PhiGlobals.StrToByteArray("SPV1");
    private const int PACKET_ID_LEN = 4;

    // offsets in packetData
    private const int OFF_SIGN = 0;
    private const int OFF_PACKET_ID = OFF_SIGN + PACKET_SIGN_LEN;
    private const int OFF_IMAGE_DATA = OFF_PACKET_ID + PACKET_ID_LEN;
    private const int OFF_JOINT_POS_DATA = OFF_IMAGE_DATA + PhiGlobals.NUM_IMAGE_ELEM;
    private const int OFF_GYRO_DATA = OFF_JOINT_POS_DATA + (PhiGlobals.NUM_JOINT_POS_ELEM * sizeof(float));
    private const int OFF_ACCEL_DATA = OFF_GYRO_DATA + (PhiGlobals.NUM_GYRO_ELEM * sizeof(float));
    private const int OFF_TEMP_DATA = OFF_ACCEL_DATA + (PhiGlobals.NUM_ACCEL_ELEM * sizeof(float));
    private const int OFF_PACKET_END = OFF_TEMP_DATA + PhiGlobals.NUM_TEMP_ELEM;

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

    internal double getPitch() {
      double raw = (double) PhiGlobals.MAKE_FLOAT(packetData, OFF_GYRO_DATA);
      return raw;
    }

    internal double getYaw() {
      double raw = (double)PhiGlobals.MAKE_FLOAT(packetData, OFF_GYRO_DATA + 4);
      return raw;
    }

    internal double getRoll() {
      double raw = (double)PhiGlobals.MAKE_FLOAT(packetData, OFF_GYRO_DATA + 8);
      return raw;
    }

    internal void getJointData(float[] joints) {
      for (int i = 0 ; i < PhiGlobals.NUM_MOTOR_ELEM ; i++) {
        joints[i] = PhiGlobals.MAKE_FLOAT(packetData, OFF_JOINT_POS_DATA + (i * sizeof(float)));
      }
    }

    internal UInt32 getPacketID() {
      UInt32 raw = PhiGlobals.MAKE_UINT32(packetData, OFF_PACKET_ID);
      return raw;
    }

  } // class
} // namespace
