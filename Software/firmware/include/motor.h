
//
// Abstract motor defs for spiderDroid (Phi)
//

/*

   Motors are connected to Polulu 2s9v1 controllers.  Several
   controllers have their RX pins wired together and they all receive
   the command regardless of motor.  The command specifies the motor ID
   however so all controllers ignore the command except the one that
   has a motor with the correct ID.

   Note: this means that the controllers must be programmed *before*
   being wired together to assign motor IDs.
   
*/

// motor names "spj":
//   s = side (left/right) =  l | r
//   p = position (front/back) = f | b
//   j = joint (hip/thigh/knee) = h | t | k
//
// controller assignment:
//
//  ctrl 0: idx 0: rft
//  ctrl 0: idx 1: rfk
//
//  ctrl 1: idx 0: lft
//  ctrl 1: idx 1: lfk
//
//  ctrl 2: idx 0: rbt
//  ctrl 2: idx 1: rbk
//
//  ctrl 3: idx 0: lbt
//  ctrl 3: idx 1: lbk
//
//  ctrl 4: idx 0: rfh
//  ctrl 4: idx 1: lfh
//
//  ctrl 5: idx 0: rbh
//  ctrl 5: idx 1: lbh
//

#define MOTOR_NAME_TO_IDX(n) \
  ( n[2] == 'h' \
    ? 8 + (n[1] == 'f' ? 0 : 2) + (n[0] == 'r' ? 0 : 1) \
    : (n[1] == 'f' ? 0 : 4) + (n[0] == 'r' ? 0 : 2) + (n[2] == 't' ? 0 : 1) )


typedef struct {
  BYTE controllerId;  // id of controller for motor
  BYTE motorIdx;      // idx of motor in controller (0 or 1)
  
} MOTOR_DEF;


// functions

void setMotorPower(int motorIdx, BYTE power, BOOL bFwd);
