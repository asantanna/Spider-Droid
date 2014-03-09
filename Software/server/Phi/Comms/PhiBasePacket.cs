using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi
{
  class PhiBasePacket {

    //
    // CONST
    //

    // camera sensor data
    internal const int IMAGE_X = 160;
    internal const int IMAGE_Y = 120;
    internal const int NUM_IMAGE_ELEM = IMAGE_X * IMAGE_Y;

    // leg motor cmds
    internal const int NUM_LEGS = 4;
    internal const int MOTORS_PER_LEG = 3;
    internal const int NUM_MOTOR_ELEM = MOTORS_PER_LEG * NUM_LEGS;

    // motor array indexes

    internal enum MOTOR_IDX {
      IDX_LFT = 0,      // motorName = A0
      IDX_LFK = 1,      // motorName = A1
      IDX_RFT = 2,      // motorName = B0
      IDX_RFK = 3,      // motorName = B1
      IDX_LBT = 4,      // motorName = C0
      IDX_LBK = 5,      // motorName = C1
      IDX_RBT = 6,      // motorName = D0
      IDX_RBK = 7,      // motorName = D1
      IDX_LFH = 0,      // motorName = E0
      IDX_RFH = 0,      // motorName = E1
      IDX_LBH = 0,      // motorName = F0
      IDX_RBH = 0,      // motorName = F1 
    };

    // misc sensor data
    internal const int NUM_JOINT_POS_ELEM = NUM_MOTOR_ELEM;
    internal const int NUM_GYRO_ELEM = 3;
    internal const int NUM_ACCEL_ELEM = 3;
    internal const int NUM_TEMP_ELEM = 1;

    // description of packet fields - passed to ctor
    internal class PACKET_FIELD_DEF {
      internal string name;
      internal int byteLen;
      internal PACKET_FIELD_DEF(string _name, int _byteLen) { name = _name; byteLen = _byteLen; }
    };

    internal int[] fieldOffs;

    //
    // VARS
    //

    internal int Length {
      get {
        return packetData.Length;
      }
    }

    internal byte[] packetData;

    //
    // CODE
    //

    internal PhiBasePacket(PACKET_FIELD_DEF[] fieldDefs) {

      // compute field offsets
      
      fieldOffs = new int[fieldDefs.Length];
      int off = 0;
      
      for (int i = 0 ; i < (int) fieldDefs.Length ; i++) {
        fieldOffs[i] = off;
        off += fieldDefs[i].byteLen;
      }

      // alloc state packet
      packetData = new byte[off];
    }
    
  } // class
} // namespace