
//
// Interface to PHI's motors
//

#include "phi.h"

MOTOR_DEF motorDefs[] = {
  { 0, 0},    // rft
  { 0, 1},    // rfk
  { 1, 0},    // lft
  { 1, 1},    // lfk
  { 2, 0},    // rbt
  { 2, 1},    // rbk
  { 3, 0},    // lbt
  { 3, 1},    // lbk
  { 4, 0},    // rfh
  { 4, 1},    // lfh
  { 5, 0},    // rbh
  { 5, 1},    // lbh
};

void setMotorPower(int motorIdx, BYTE power, BOOL bFwd) {

  MOTOR_DEF* md = &motorDefs[motorIdx];

  BYTE cmd = 
    bFwd ? (md -> motorIdx == 0 ? MC_CMD_FWD_M0 : MC_CMD_FWD_M1)
         : (md -> motorIdx == 0 ? MC_CMD_BCK_M0 : MC_CMD_BCK_M1);
  
  char motorCmd[] = {
    MC_CMD_SIGN,
    /*HACK FOR NOW md -> controllerID,*/ 9,
    cmd,
    /*ANOTHER HACK until motor is init properly */ power > 127 ? 127 : power
  };
    
  uartSend(motorCmd, sizeof(motorCmd));
}

