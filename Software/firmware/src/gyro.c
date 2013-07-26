//
// Interface to PHI's gyroscope
//

#include "phi.h"

BOOL gyroInit() {

  BOOL rc = FALSE;
  BYTE txData[2];
  BYTE rxData[2];

  // make sure gyro is there
  txData[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_WHOAMI_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txData, 1, rxData, 1);
  
  if (rxData[0] != GYRO_WHOAMI_REPLY) {
    // bad (or no) reply
    LOG_ERR("bad (or no) reply from gyroscope, reply=%02Xh", rxData[0]);
    goto quick_exit;
  }

  // CR2 - HACK - leave at default values?
  
  // CR3 - default values OK
  
  // CR4
  // Note: bits of a byte are sent out MSB but multiple bytes are sent out LSB (little endian)
  
  txData[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR4_ADDR;
  txData[1] = GYRO_CR4_BDU_EN | GYRO_CR4_LITTLE_ENDIAN | GYRO_CR4_FS_250DPS;
  spi_send(GYRO_SPI_IDX, txData, 2);

  // CR5 - default values OK

  // REFERENCE - not used

  // INT1_THS - not used

  // INT1_DURATION - not used

  // INT1_CFG - default values OK (ints disabled)

  // CR1
  txData[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR1_ADDR;
  txData[1] = GYRO_CR1_DR_100HZ | GYRO_CR1_BW_CO_1 | GYRO_CR1_X_EN | GYRO_CR1_Y_EN | GYRO_CR1_Z_EN;
  spi_send(GYRO_SPI_IDX, txData, 2);

  // success
  rc = TRUE;

quick_exit:
  
  return rc;
}

BYTE gyroReadStatus() {
  
  BYTE txData[1];
  BYTE rxData[1];
  
  txData[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_STATUS_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txData, 1, rxData, 1);

  return rxData[0];
}

INT16 gyroReadRawDps(BYTE lowRegAddr) {
  BYTE txData[1];
  BYTE rxData[2];

  txData[0] = GYRO_ADDR_READ | GYRO_ADDR_AUTO_INC | lowRegAddr;
  spi_sendreceive(GYRO_SPI_IDX, txData, 1, rxData, 2);

  return *((INT16 *)rxData);
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
  BYTE txData[1];
  BYTE rxData[2];
  
  txData[0] = GYRO_ADDR_READ | GYRO_ADDR_AUTO_INC | lowRegAddr;
  spi_sendreceive(GYRO_SPI_IDX, txData, 1, rxData, 2);

  INT16 raw = gyroReadRawDps(lowRegAddr);
  
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
// Note: gyro is sampling at 100Hz and we *know* we wont read that fast so
// there will be overrun.  Therefore, we don't bother checking for it.

static float prevPitchDps = 0;
static float prevYawDps = 0;
static float prevRollDps = 0;

void gyroGetData(float* pPitchDps, float* pYawDps, float* pRollDps) {
  
  BYTE status = gyroReadStatus();

  if ((status & GYRO_STATUS_Y_AVAIL) != 0) {
    // have new Y (pitch) data
    prevPitchDps = gyroReadDps(GYRO_YL_ADDR);
  }

  if ((status & GYRO_STATUS_Z_AVAIL) != 0) {
    // have new Z (yaw) data
    prevYawDps = gyroReadDps(GYRO_ZL_ADDR);
  }

  if ((status & GYRO_STATUS_X_AVAIL) != 0) {
    // have new X (roll) data
    prevRollDps = gyroReadDps(GYRO_XL_ADDR);
  }

  // copy back
  
  *pPitchDps = prevPitchDps;
  *pYawDps   = prevYawDps;
  *pRollDps  = prevRollDps;
  
}

INT8 gyroGetTemp() {
  BYTE txData[1];
  BYTE rxData[1];

  txData[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_TEMP_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txData, 1, rxData, 1);

  return rxData[0];
}

