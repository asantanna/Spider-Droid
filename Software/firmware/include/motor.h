
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

typedef struct {
  BYTE motorID;     // id of motor in Polulu 2s9v1 controller(s)
  
} MOTOR_DEF;
