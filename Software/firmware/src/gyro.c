//
// Interface to PHI's gyroscope
//

#include "phi.h"

BOOL PHI_gyroInit() {

  BOOL rc = FALSE;
  BYTE txBuff[2];
  BYTE rxBuff[2];

  // make sure gyro is there
  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_WHOAMI_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txBuff, 1, rxBuff, 1);

  if (rxBuff[0] != GYRO_WHOAMI_REPLY) {
    // bad (or no) reply
    LOG_ERR("bad (or no) reply from gyroscope, reply=%02Xh", rxBuff[0]);
    goto quick_exit;
  }

  // CR2 - leave at default values

  // CR3 - default values OK

  // CR4
  // Note: bits of a byte are sent out MSB but multiple bytes are sent out LSB (little endian)

  txBuff[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR4_ADDR;
  txBuff[1] = GYRO_CR4_BDU_EN | GYRO_CR4_LITTLE_ENDIAN | GYRO_CR4_FS_250DPS;
  spi_send(GYRO_SPI_IDX, txBuff, 2);

  // CR5 - default values OK

  // REFERENCE - not used

  // INT1_THS - not used

  // INT1_DURATION - not used

  // INT1_CFG - default values OK (ints disabled)

  // CR1
  txBuff[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR1_ADDR;
  txBuff[1] = GYRO_CR1_DR_100HZ | GYRO_CR1_BW_CO_1 | GYRO_CR1_MODE_NORMAL | GYRO_CR1_X_EN | GYRO_CR1_Y_EN | GYRO_CR1_Z_EN;
  spi_send(GYRO_SPI_IDX, txBuff, 2);

  // success
  rc = TRUE;

quick_exit:
  
  return rc;
}

BYTE gyroReadStatus() {
  
  BYTE txBuff[1];
  BYTE rxBuff[1];
  
  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_STATUS_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txBuff, 1, rxBuff, 1);

  return rxBuff[0];
}

INT16 gyroReadRawDps(BYTE lowRegAddr) {
  BYTE txBuff[1];
  BYTE rxBuff[2];

  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_AUTO_INC | lowRegAddr;
  spi_sendreceive(GYRO_SPI_IDX, txBuff, 1, rxBuff, 2);

  return *((INT16 *)rxBuff);
}

// gyroscope can have a systematic error and thus zero dps might not
// get reported as 0.  This function should be called with a *stationary*
// gyroscope so it can calculate averages of readings so we can remove
// this error.
//
// Note: according to doc this is only necessary for very precise measurements.
//
// Note 2: Additionally, in PHI, number *values* are meaningless, they are
// considered ordered symbols only so systematic errors are irrelevant.
// This function will never get called.
//
// Note 3: We also don't care about noise and thus don't have threshold logic.
// This is because PHI will equalize all sensors values and quantize.
// Therefore the noise will automatically be quantized away.


/*
static INT16 xZeroRate = 0;
static INT16 yZeroRate = 0;
static INT16 zZeroRate = 0;

void gyroCalcZeroRates(BYTE lowRegAddr) {
  // not implemented because PHI has no need for it
}
*/

float gyroReadDps(BYTE lowRegAddr) {

  INT16 raw = gyroReadRawDps(lowRegAddr);

  // LOG_INFO("raw %c dps = %d", 'X' + (char)((lowRegAddr - GYRO_XL_ADDR) / 2), raw);

  // see note above about why we don't substract the zeroRate
  float trueDps = raw * GYRO_250DPS_MULT;

  return trueDps;
}

//
// Gyro has three axes: x, y and z (as labeled on board). The reported values
// relate to rotation about these axes.  Regarding PHI, the board  has a horizontal
// mounting, with the connector pins going in the left-right axis.  Therefore,
// x = roll, y = pitch and z = yaw.
//
// Note: this function returns DPS (degrees per second)
//

TODO("must enable FIFO because we are too slow reading")
TODO("check for overrun")

void PHI_gyroGetData(float* pPitchDps, float* pYawDps, float* pRollDps) {
  
  float pitchDps = 0;
  float yawDps = 0;
  float rollDps = 0;
  
  BYTE status = gyroReadStatus();

  // LOG_INFO("gyro status = %02Xh", status);

  if ((status & GYRO_STATUS_Y_AVAIL) != 0) {
    // have new Y (pitch) data
    pitchDps = gyroReadDps(GYRO_YL_ADDR);
  }

  if ((status & GYRO_STATUS_Z_AVAIL) != 0) {
    // have new Z (yaw) data
    yawDps = gyroReadDps(GYRO_ZL_ADDR);
  }

  if ((status & GYRO_STATUS_X_AVAIL) != 0) {
    // have new X (roll) data
    rollDps = gyroReadDps(GYRO_XL_ADDR);
  }

  // copy back
  
  *pPitchDps = pitchDps;
  *pYawDps   = yawDps;
  *pRollDps  = rollDps;
}

INT8 PHI_gyroGetTemp() {
  BYTE txBuff[1];
  BYTE rxBuff[1];

  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_TEMP_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txBuff, 1, rxBuff, 1);

  return rxBuff[0];
}

