﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi
{
  static class PhiGlobals
  {
    //
    // CONST
    //

    // version
    internal static byte[] PHI_VERSION = StrToByteArray("0.9 (exp)");

    // leg motor cmds
    internal const int NUM_LEGS = 4;
    internal const int MOTORS_PER_LEG = 3;
    internal const int NUM_MOTOR_ELEM = MOTORS_PER_LEG * NUM_LEGS;

    // camera sensor data
    internal const int IMAGE_X = 160;
    internal const int IMAGE_Y = 120;
    internal const int NUM_IMAGE_ELEM = IMAGE_X * IMAGE_Y;

    // misc sensor data
    internal const int NUM_JOINT_POS_ELEM = NUM_LEGS * MOTORS_PER_LEG;
    internal const int NUM_GYRO_ELEM = 3;
    internal const int NUM_ACCEL_ELEM = 3;
    internal const int NUM_TEMP_ELEM = 1;

    //
    // GLOBAL VARS
    //

    internal static bool bInit = false;
    internal static MainWindow mainWindow = null;
    internal static System.Text.UTF8Encoding encoding;

    //
    // GLOBAL FUNCTIONS
    //

    internal static void phiAbort(string message)
    {
      Console.WriteLine("*");
      Console.WriteLine("* Phi-Core ABORT");
      Console.WriteLine("*");
      Console.WriteLine("");
      Console.WriteLine(message);

      // TODO - HACK - do something appropriate here
    }

    internal static byte[] StrToByteArray(string str)
    {
      if (encoding == null) {
        encoding = new System.Text.UTF8Encoding();
      }

      return encoding.GetBytes(str);
    }

    internal static Int16 MAKE_INT16(byte[] ba, int offset) {
      Int16[] s = new Int16[1];
      Buffer.BlockCopy(ba, offset, s, 0, 2);
      return s[0];
    }

    internal static UInt16 MAKE_UINT16(byte[] ba, int offset) {
      UInt16[] s = new UInt16[1];
      Buffer.BlockCopy(ba, offset, s, 0, 2);
      return s[0];
    }

    internal static UInt32 MAKE_UINT32(byte[] ba, int offset) {
      UInt32[] s = new UInt32[1];
      Buffer.BlockCopy(ba, offset, s, 0, 4);
      return s[0];
    }

    internal static float MAKE_FLOAT(byte[] ba, int offset) {
      float[] f = new float[1];
      Buffer.BlockCopy(ba, offset, f, 0, 4);
      return f[0];
    }

  } // class
} // namespace
