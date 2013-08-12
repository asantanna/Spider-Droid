﻿using System;
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
    private static byte[] STATE_PACKET_SIGN = PhiGlobals.StrToByteArray("SPV1");

    // offsets in packetData
    private const int OFF_SIGN = 0;
    private const int OFF_VISION_DATA = OFF_SIGN + PACKET_SIGN_LEN;
    private const int OFF_GYRO_DATA = OFF_VISION_DATA + PhiGlobals.NUM_VISION_ELEM;
    private const int OFF_JOINT_POS_DATA = OFF_GYRO_DATA + (PhiGlobals.NUM_GYRO_ELEM * sizeof(float));
    private const int OFF_TEMP_DATA = OFF_JOINT_POS_DATA + PhiGlobals.NUM_JOINT_POS_ELEM;
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

    internal double getPitchDelta() {
      double raw = (double) PhiGlobals.MAKE_FLOAT(packetData, OFF_GYRO_DATA);
      return raw;
    }

    internal double getYawDelta() {
      double raw = (double)PhiGlobals.MAKE_FLOAT(packetData, OFF_GYRO_DATA + 4);
      return raw;
    }

    internal double getRollDelta() {
      double raw = (double)PhiGlobals.MAKE_FLOAT(packetData, OFF_GYRO_DATA + 8);
      return raw;
    }

  } // class
} // namespace
