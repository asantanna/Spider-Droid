
//
// Interface to PHI's motors
//

#include "phi.h"

// See motor.h for more info on motor names, etc.

//
// Init motor controllers
//

BOOL initMotorCtrl() {
  LOG_FATAL("NOT IMPLEMENTED");
  // set PWM type, failure modes, bla bla
}

//
// Set motor power by sending a command to the appropriate
// motor controller through the UART
//
// Note: power is [0, 127]

void PHI_setMotorPower(char motorName[2], BYTE power, BOOL bFwd) {

  int ctrlID = MOTOR_NAME_TO_CTRL_ID (motorName);
  int selIdx = MOTOR_NAME_TO_SEL_IDX (motorName);

  BYTE cmd = 
    bFwd ? (selIdx == 0 ? MC_CMD_FWD_M0 : MC_CMD_FWD_M1)
         : (selIdx == 0 ? MC_CMD_BCK_M0 : MC_CMD_BCK_M1);

  char motorCmd[] = {
    MC_CMD_SIGN,
    ctrlID,
    cmd,
    power > 127 ? 127 : power
  };

  uart_send(motorCmd, sizeof(motorCmd));
}

//
// Get motor (aka joint) position by sending a command to the
// appropriate ADC through SPI 1
//

UINT16 PHI_getJointPosition(char motorName[2]) {
  
  int adcIdx = MOTOR_NAME_TO_ADC_IDX (motorName);
  
  BYTE txBuff[3];
  BYTE rxBuff[3] = {0};

  txBuff[0] = ADC_CMD1_START;
  txBuff[1] = ADC_CMD2_SINGLE | ( (((BYTE)adcIdx) << 4) & ADC_CMD2_ADDR_MASK) ;
  txBuff[2] = 0;
  
  spi_exchange(ADC_SPI_IDX, txBuff, rxBuff, COUNTOF(txBuff));

  // extract raw value [0, 1023]
  UINT16 adcValue = (UINT16) (rxBuff[1] & ADC_DATA2_MASK);
  adcValue <<= 8;
  adcValue |= (UINT16) rxBuff[2];

  // DEBUG
  // LOG_INFO("getJointPosition(%d) = %02Xh", motorIdx, adcValue);
  // LOG_INFO("  outgoing:  %02Xh %02Xh %02Xh", txBuff[0], txBuff[1], txBuff[2]); 
  // LOG_INFO("  incoming:  %02Xh %02Xh %02Xh", rxBuff[0], rxBuff[1], rxBuff[2]);

  return adcValue;
}


// Controllers come from factory with ID=9, we have to set each one
// to a different value

void PHI_setControllerId(char oldId, char newId) {

    char motorCmd[] = {
    MC_CMD_SIGN,
    oldId,
    MC_CMD_SET_CONF,
    MC_SCONF_PARAM_DEVICE_ID,
    newId,
    MC_SCONF_END_VAL_0,
    MC_SCONF_END_VAL_1
  };

  uart_send(motorCmd, sizeof(motorCmd));
}
