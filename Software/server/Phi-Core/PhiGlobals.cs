using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi_Core
{
  static class PhiGlobals
  {
    //
    // CONST
    //

    // version
    internal static byte[] PHI_VERSION = StrToByteArray("0.9 (exp)");

    // leg motor axons
    internal const int NUM_LEGS = 4;
    internal const int MOTORS_PER_LEG = 3;
    internal const int NUM_MOTOR_ELEM = MOTORS_PER_LEG * NUM_LEGS;

    // joint sensor DATA
    internal const int NUM_JOINT_POS_ELEM = NUM_LEGS * MOTORS_PER_LEG;

    // gyro sensor DATA
    // TODO: separate pos and neg components?  seems like a good idea to make
    // things easier to discover
    internal const int NUM_GYRO_ELEM = 3;

    // temp sensor DATA
    internal const int NUM_TEMP_ELEM = 1;

    // vision sensor DATA
    internal const int VISION_X = 160;
    internal const int VISION_Y = 120;
    internal const int NUM_VISION_ELEM = 4; // TODO - HACK -  VISION_X * VISION_Y;

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

    internal static short MAKE_SHORT(byte[] ba, int offset) {
      short[] s = new short[1];
      Buffer.BlockCopy(ba, offset, s, 0, 2);
      return s[0];
    }

    internal static float MAKE_FLOAT(byte[] ba, int offset) {
      float[] f = new float[1];
      Buffer.BlockCopy(ba, offset, f, 0, 4);
      return f[0];
    }

  } // class
} // namespace
