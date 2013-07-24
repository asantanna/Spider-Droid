
//
// Interface to PHI's motors
//

#include "phi.h"

void setMotorPower(MOTOR_DEF* md, BYTE power, BOOL bFwd) {

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

