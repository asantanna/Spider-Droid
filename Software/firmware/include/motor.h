
//
// Abstract motor defs for spiderDroid (Phi)
//

/*
   PHI has 12 motors, 3 per leg

   Motors are connected to Polulu 2s9v1 controllers.  All controllers
   have their RX pins wired together and they all receive every
   command regardless of motor.  The command specifies the controller ID
   however so all controllers ignore the command except the one that
   has the correct ID.

   Note: this means that the controllers must be programmed *before*
   being wired together to assign motor IDs.

   Note 2: Since wiring together the TX pins of the controllers is not
   supported (sigh), PHI cannot receive replies from the controllers.
   Commands are sent fire and pray.
   
*/

#define NUM_MOTOR_CTRL    6
#define NUM_MOTORS        (NUM_MOTOR_CTRL * 2)    // 12

//
// Motor names are of type char[2]:
//   char[0]: controller ID '[A-F]'
//   char[1]: motor select within a controller '[0-1]'
//
// Their function in the hardware is represented in comments
// below as a 3 letter code 'ABC', where:
//
//  A:  (R)ight, (L)eft
//  B:  (F)ront, (B)ack
//  C:  (H)ip, (T)high, (K)nee
//
// PHI is wired as follows:
//
//                            Front
//                    \                   /
//  A0: LFT            A1               B1
//  A1: LFK              \             /
//  B0: RFT               A0         B0
//  B1: RFK                 \       /
//  C0: LBT                  E0   E1
//  C1: LBK         Left                  Right
//  D0: RBT                  F0   F1
//  D1: RBK                 /       \
//  E0: LFH               C0         D0
//  E1: RFH              /             \
//  F0: LBH            C1               D1
//  F1: RBH           /        BACK       \
//
//

#define MOTOR_NAME_TO_CTRL_ID(name)             ((BYTE)(name[0] - 'A'))
#define MOTOR_NAME_TO_SEL_IDX(name)             ((BYTE)(name[1] - '0'))
#define MOTOR_ID_TO_ARRAY_IDX(ctrlID, selIdx)   ( ((ctrlID) * 2) + (selIdx))

// joints
#define MOTOR_ID_TO_JOINT_IDX(ctrlID, selIdx)   MOTOR_ID_TO_ARRAY_IDX(ctrlID, selIdx)
#define JOINT_IDX_TO_ADC_IDX(jIdx)              ( (jIdx) / 8)
#define JOINT_IDX_TO_ADC_CHAN(jIdx)             ( (jIdx) % 8)

//
// ADC
//
// Note: ADC functions use and return values in the canonical range [0,1]
//

#define ADC_MIN_RAW                             ((UINT16) 0)
#define ADC_MAX_RAW                             ((UINT16) 1023)
#define ADC_RAW_RANGE                           (ADC_MAX_RAW - ADC_MIN_RAW + 1)

#define ADC_RAW_TO_CANON(rawVal)                ( ((rawVal) % ADC_RAW_RANGE) / ((float)ADC_RAW_RANGE) )
#define ADC_CANON_TO_RAW(canonVal)              ( (UINT16) ((canonVal) * ((float)ADC_RAW_RANGE)) )

#define ADC_ABS_RAW_DIFF(v1, v2)                ( ( (UINT16) ( ((INT16)(v1)) - ((INT16)(v2)) ) ) % ADC_RAW_RANGE )


