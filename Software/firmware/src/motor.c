
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

//
// Set motor power by sending a command to the appropriate
// motor controller through the UART
// 

void PHI_setMotorPower(int motorIdx, BYTE power, BOOL bFwd) {

  MOTOR_DEF* md = &motorDefs[motorIdx];

  BYTE cmd = 
    bFwd ? (md -> motorIdx == 0 ? MC_CMD_FWD_M0 : MC_CMD_FWD_M1)
         : (md -> motorIdx == 0 ? MC_CMD_BCK_M0 : MC_CMD_BCK_M1);

  WARN("Ignoring motor controller ID")
  TODO("Do we want 7-bit or 8-bit motor speeds?")
    
  char motorCmd[] = {
    MC_CMD_SIGN,
    /*HACK FOR NOW md -> controllerID,*/ 9,
    cmd,
    /*ANOTHER HACK until motor is init properly */ power > 127 ? 127 : power
  };
    
  uart_send(motorCmd, sizeof(motorCmd));
}

//
// Get motor (aka joint) position by sending a command to the
// appropriate ADC through SPI 1
//

UINT16 PHI_getMotorPosition(int motorIdx) {
  
  BYTE txBuff[3];
  BYTE rxBuff[3] = {0};

  txBuff[0] = ADC_CMD1_START;
  txBuff[1] = ADC_CMD2_SINGLE | ( (((BYTE)motorIdx) << 4) & ADC_CMD2_ADDR_MASK) ;
  txBuff[2] = 0;
  
  spi_exchange(ADC_SPI_IDX, txBuff, rxBuff, 1);

  // extract raw value [0, 1023]
  UINT16 adcValue = (UINT16) (rxBuff[1] & ADC_DATA2_MASK);
  adcValue <<= 8;
  adcValue |= (UINT16) rxBuff[2];

  // DEBUG
  LOG_INFO("getMotorPosition(%d) = %02Xh", motorIdx, adcValue);
  LOG_INFO("  outgoing:  %02Xh %02Xh %02Xh", txBuff[0], txBuff[1], txBuff[2]); 
  LOG_INFO("  incoming:  %02Xh %02Xh %02Xh", rxBuff[0], rxBuff[1], rxBuff[2]);

  return adcValue;
}


