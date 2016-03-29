using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi
{
  public class PhiBasePacket {

    //
    // CONST
    //

    // camera sensor data
    // HACK: size reduced for now because of network latency
    public const int IMAGE_X = 8; // 160;
    public const int IMAGE_Y = 8; // 120;
    public const int NUM_IMAGE_ELEM = IMAGE_X * IMAGE_Y;

    // leg motor cmds
    public const int NUM_LEGS = 4;
    public const int MOTORS_PER_LEG = 3;
    public const int NUM_MOTOR_ELEM = MOTORS_PER_LEG * NUM_LEGS;

    // motor array indexes
    // Note: not used but useful documentation

    public enum MOTOR_IDX {
      IDX_LFT = 0,      // motorName = A0
      IDX_LFS = 1,      // motorName = A1
      IDX_RFT = 2,      // motorName = B0
      IDX_RFS = 3,      // motorName = B1
      IDX_LBT = 4,      // motorName = C0
      IDX_LBS = 5,      // motorName = C1
      IDX_RBT = 6,      // motorName = D0
      IDX_RBS = 7,      // motorName = D1
      IDX_LFH = 0,      // motorName = E0
      IDX_RFH = 0,      // motorName = E1
      IDX_LBH = 0,      // motorName = F0
      IDX_RBH = 0,      // motorName = F1 
    };

    // misc sensor data
    public const int NUM_JOINT_POS_ELEM = NUM_MOTOR_ELEM;
    public const int NUM_GYRO_ELEM = 3;
    public const int NUM_ACCEL_ELEM = 3;
    public const int NUM_TEMP_ELEM = 1;

    // description of packet fields - passed to ctor
    public class PACKET_FIELD_DEF {
      public string name;
      public int byteLen;
      public PACKET_FIELD_DEF(string _name, int _byteLen) { name = _name; byteLen = _byteLen; }
    };

    public int[] fieldOffs;

    //
    // VARS
    //

    public int Length {
      get {
        return packetData.Length;
      }
    }

    public byte[] packetData;

    //
    // CODE
    //

    public PhiBasePacket(PACKET_FIELD_DEF[] fieldDefs) {

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