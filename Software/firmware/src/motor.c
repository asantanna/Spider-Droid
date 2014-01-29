
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

void PHI_setMotorPower(BYTE ctrlID, BYTE selIdx, BYTE power, BOOL bFwd) {

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
// Stop motors
//

void stopMotor(BYTE ctrlID, BYTE selIdx) {
  PHI_setMotorPower(ctrlID, selIdx, 0, TRUE);
}

void stopAllMotors() {
  BYTE ctrlID;
  BYTE selIdx;

  // discard everything queued
  uart_discardAll();

  // go through each controller
  for (ctrlID = 'A' ; ctrlID <= 'F' ; ctrlID ++) {
    // go through each motor of this controller
    for (selIdx = '0' ; selIdx <= '1' ; selIdx ++) {
      // stop motor
      stopMotor(ctrlID, selIdx);
    }
  }
}

//
// Flush motor commands
//

void flushMotorCmds() {
  uart_flush();
}

//
// Get joint position by sending a command to the appropriate ADC through SPI 1
//

UINT16 PHI_getRawJointPos(BYTE adcIdx) {
  
  BYTE txBuff[3];
  BYTE rxBuff[3] = {0};

  txBuff[0] = ADC_CMD1_START;
  txBuff[1] = ADC_CMD2_SINGLE | ( ((adcIdx) << 4) & ADC_CMD2_ADDR_MASK) ;
  txBuff[2] = 0;
  
  spi_exchange(ADC_SPI_IDX, txBuff, rxBuff, COUNTOF(txBuff));

  // extract raw value [0, 1023]
  UINT16 adcValue = (UINT16) (rxBuff[1] & ADC_DATA2_MASK);
  adcValue <<= 8;
  adcValue |= (UINT16) rxBuff[2];

  // DEBUG
  // LOG_INFO("getRawJointPos(%d) = %02Xh", adcIdx, adcValue);
  // LOG_INFO("  outgoing:  %02Xh %02Xh %02Xh", txBuff[0], txBuff[1], txBuff[2]); 
  // LOG_INFO("  incoming:  %02Xh %02Xh %02Xh", rxBuff[0], rxBuff[1], rxBuff[2]);

  return adcValue;
}

float getJointPos(BYTE adcIdx) {
  // convert raw ADC val to degrees
  return ADC_VAL_TO_DEG( HAL_getRawJointPos(adcIdx) );
}

float getJointPosByMotorID(BYTE ctrlID, BYTE selIdx) {
  // convert raw ADC val to degrees
  return getJointPos(MOTOR_ID_TO_ADC_IDX(ctrlID, selIdx));
}


// Controllers come from factory with ID=9, we have to set each one
// to a different value

void PHI_setControllerId(BYTE oldId, BYTE newId) {

    char motorCmd[] = {
    MC_CMD_SIGN,
    (char) oldId,
    MC_CMD_SET_CONF,
    MC_SCONF_PARAM_DEVICE_ID,
    (char) newId,
    MC_SCONF_END_VAL_0,
    MC_SCONF_END_VAL_1
  };

  uart_send(motorCmd, sizeof(motorCmd));
}
