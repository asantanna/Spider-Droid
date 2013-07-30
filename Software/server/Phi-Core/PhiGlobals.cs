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
    internal const int NUM_MOTOR_AXONS = MOTORS_PER_LEG * NUM_LEGS;

    // joint sensor axons
    internal const int NUM_JOINT_POS_AXONS = NUM_LEGS * MOTORS_PER_LEG;

    // gyro sensor axons
    // TODO: separate pos and neg components?  seems like a good idea to make
    // things easier to discover
    internal const int NUM_GYRO_AXONS = 3;

    // temp sensor axons
    internal const int NUM_TEMP_AXONS = 1;

    // vision sensor axons
    internal const int VISION_X = 160;
    internal const int VISION_Y = 120;
    internal const int NUM_VISION_AXONS = VISION_X * VISION_Y;

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



  } // class
} // namespace
