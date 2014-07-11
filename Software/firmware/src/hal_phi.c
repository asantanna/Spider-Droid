//
// PHI HAL
//

#include <phi.h>


char* HAL_PHI_initPeripherals() {

  // NULL means success
  char *rc = NULL;

  g_initPeriph = FALSE;

  // init GPIO memory map (do this first)
  if (!gpio_init()) {
    rc = "HAL_PHI_InitPeripherals: GPIO init failed";
    goto quick_exit;
  }

  // set up UART for communication with motor controllers
  if (!uart_init()) {
    rc = "HAL_PHI_InitPeripherals: UART init failed";
    goto quick_exit;
  }

  // set up SPI for communication with gyroscope and ADCs
  if (!spi_init()) {
    rc = "HAL_PHI_InitPeripherals: SPI init failed";
    goto quick_exit;
  }

  // set up I2C for communication with accelerometer
  TODO("i2c_init not implemented");

  // init gyroscope (FIFO enabled)
  if (!gyroInit(TRUE)) {
    rc = "HAL_PHI_InitPeripherals: gyroscope init failed";
    goto quick_exit;
  }

  // init accelerometer
  TODO("accelInit() nit not implemented");

  // init motor controllers
  if (!initMotorCtrl()) {
    rc = "HAL_PHI_InitPeripherals: motor ctrl init failed";
    goto quick_exit;
  }

  // success
  g_initPeriph = TRUE;

quick_exit:

  return rc;
}

void HAL_PHI_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {

  // call func that returns in degrees
  gyroGetDeltaDegrees(pPitchDelta, pYawDelta, pRollDelta);
  
  // convert [-360,360] degrees to [-1,1]
  *pPitchDelta /=  360.0f;
  *pYawDelta   /=  360.0f;
  *pRollDelta  /=  360.0f;
}

float HAL_PHI_gyroGetTemp() {
  return GYRO_TEMP_RAW_TO_CANON(gyroGetRawTemp());
}

void HAL_PHI_setMotorPower(BYTE ctrlID, BYTE selIdx, BYTE power, BOOL bFwd) {
  setMotorPower(ctrlID, selIdx, power, bFwd);
}

void HAL_PHI_setControllerId(BYTE oldId, BYTE newId) {
  setControllerId(oldId, newId);
}

float HAL_PHI_getJointPos(BYTE jointIdx) {
  // convert raw ADC val to canonical
  return ADC_RAW_TO_CANON(getRawJointPos(jointIdx) );
}
