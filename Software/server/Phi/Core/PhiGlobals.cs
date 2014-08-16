using Phi.UI;
using Phi.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi
{
  static partial class PhiGlobals
  {
    //
    // CONST
    //

    // version
    public static byte[] PHI_VERSION = StrToByteArray("0.9 (exp)");

    //
    // GLOBAL VARS
    //

    public static bool bInit = false;
    public static MainWindow mainWindow = null;
    public static LogForm logForm = null;
    public static System.Text.UTF8Encoding encoding;

    // model of PHI
    public static PhiModel model;

    //
    // GLOBAL FUNCTIONS
    //

    public static void phiAbort(string message)
    {
      Console.WriteLine("*");
      Console.WriteLine("* Phi-Core ABORT");
      Console.WriteLine("*");
      Console.WriteLine("");
      Console.WriteLine(message);

      // TODO - HACK - do something appropriate here
    }

    public static byte[] StrToByteArray(string str)
    {
      if (encoding == null) {
        encoding = new System.Text.UTF8Encoding();
      }

      return encoding.GetBytes(str);
    }

    //
    // Methods to read and write to comm buffers
    // Note: endianness is reversed
    //

    static UInt16[] _u16 = new UInt16[1];
    static UInt32[] _u32 = new UInt32[1];
    static UInt64[] _u64 = new UInt64[1];
    static float[]  _f   = new float[1];

    public static UInt16 READ_UINT16(byte[] ba, int offset) {
      Buffer.BlockCopy(ba, offset, _u16, 0, 2);
      return _u16[0];
    }

    public static UInt32 READ_UINT32(byte[] ba, int offset) {
      Buffer.BlockCopy(ba, offset, _u32, 0, 4);
      return _u32[0];
    }

    public static UInt64 READ_UINT64(byte[] ba, int offset) {
      Buffer.BlockCopy(ba, offset, _u64, 0, 8);
      return _u64[0];
    }

    public static float READ_FLOAT(byte[] ba, int offset) {
      Buffer.BlockCopy(ba, offset, _f, 0, 4);
      return _f[0];
    }

    public static void WRITE_FLOAT(float val, byte[] ba, int offset) {
      _f[0] = val;
      Buffer.BlockCopy(_f, 0, ba, offset, 4);
    }

    // approximate comparison
    public static bool approxEq(double a, double b, double eps) {
      return (Math.Abs(a-b) <= eps);
    }

    //
    // Class test suites
    //

    static public void runClassTests() {

      Console.WriteLine("***");
      Console.WriteLine("*** Starting class tests");
      Console.WriteLine("***");

      // generic classes have test suites in PhiGlobals to make things less messy in the debugger
      PhiGlobals.runClassTest_CircularBuffer();
      PhiGlobals.runClassTest_ListWithReusableElements();

      Console.WriteLine("*** Done" );
    }

    //
    // BYTE ORDER REVERSAL ROUTINES
    // Note: Intel and ARM are both little-endian so these are not needed
    //

    public static UInt16 reverseBytes(UInt16 value) {
      return (UInt16)((value & 0xFFU) << 8 | (value & 0xFF00U) >> 8);
    }

    // reverse byte order (32-bit)
    public static UInt32 reverseBytes(UInt32 value) {
      return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
             (value & 0x00FF0000U) >> 8  | (value & 0xFF000000U) >> 24;
    }

    // reverse byte order (64-bit)
    public static UInt64 reverseBytes(UInt64 value) {
      return (value & 0x00000000000000FFUL) << 56 | (value & 0x000000000000FF00UL) << 40 |
             (value & 0x0000000000FF0000UL) << 24 | (value & 0x00000000FF000000UL) << 8  |
             (value & 0x000000FF00000000UL) >> 8  | (value & 0x0000FF0000000000UL) >> 24 |
             (value & 0x00FF000000000000UL) >> 40 | (value & 0xFF00000000000000UL) >> 56;
    }

  } // PhiGlobals class
} // namespace
