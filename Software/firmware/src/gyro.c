//
// Interface to PHI's gyroscope
//

#include "phi.h"

//
// SAMPLE/RANGE selection
//
// NOTE: these pairs must be adjusted together
//

#define GYRO_MAX_VALUE          250.0f
#define GYRO_MAX_VALUE_CMD      GYRO_CR4_FS_250DPS
#define GYRO_MAX_VALUE_MULT     GYRO_250DPS_MULT

#define GYRO_SAMPLE_RATE        100.0f
#define GYRO_SAMPLE_PERIOD      (1.0f / GYRO_SAMPLE_RATE)
#define GYRO_SAMPLE_RATE_CMD    GYRO_CR1_DR_100HZ

// end of SAMPLE/RANGE selection

BOOL PHI_gyroInit(BOOL bEnableFifo) {

  BOOL rc = FALSE;
  BYTE txBuff[2];
  BYTE rxBuff[2];

  // make sure gyro is there
  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_WHOAMI_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txBuff, 1, rxBuff, 1);

  if (rxBuff[0] != GYRO_WHOAMI_REPLY) {
    // bad (or no) reply
    LOG_FATAL("bad (or no) reply from gyroscope, reply=%02Xh", rxBuff[0]);
    goto quick_exit;
  }

  // CR2 - leave at default values

  // CR3 - default values OK

  // CR4
  // Note: bits of a byte are sent out MSB but multiple bytes are sent out LSB (little endian)

  txBuff[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR4_ADDR;
  txBuff[1] = GYRO_CR4_BDU_EN | GYRO_CR4_LITTLE_ENDIAN | GYRO_MAX_VALUE_CMD;
  spi_send(GYRO_SPI_IDX, txBuff, 2);

  // CR5 - default OK unless want FIFO (32 slot)

  if (bEnableFifo == TRUE) {
    // enable FIFO, ints off, high pass filter off
    txBuff[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR5_ADDR;
    txBuff[1] = GYRO_CR5_FIFO_EN;
    spi_send(GYRO_SPI_IDX, txBuff, 2);
    // set FIFO mode=stream, no watermark interrupt
    txBuff[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_FIFO_ADDR;
    txBuff[1] = GYRO_FIFO_MODE_STREAM;
    spi_send(GYRO_SPI_IDX, txBuff, 2);
  }

  // REFERENCE - not used

  // INT1_THS - not used

  // INT1_DURATION - not used

  // INT1_CFG - default values OK (ints disabled)

  // CR1
  txBuff[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR1_ADDR;
  txBuff[1] = GYRO_SAMPLE_RATE_CMD | GYRO_CR1_BW_CO_1 | GYRO_CR1_MODE_NORMAL | GYRO_CR1_X_EN | GYRO_CR1_Y_EN | GYRO_CR1_Z_EN;
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

TODO("implement gyro calibration and thresholding")

/*
static INT16 xZeroRate = 0;
static INT16 yZeroRate = 0;
static INT16 zZeroRate = 0;

void gyroCalcZeroRates(BYTE lowRegAddr) {
  // not implemented
}
*/

float gyroReadDps(BYTE lowRegAddr) {

  INT16 raw = gyroReadRawDps(lowRegAddr);

  // LOG_INFO("raw %c dps = %d", 'X' + (char)((lowRegAddr - GYRO_XL_ADDR) / 2), raw);

  // see note above about why we don't substract the zeroRate
  float trueDps = raw * GYRO_MAX_VALUE_MULT;

  return trueDps;
}

//
// Gyro has three axes: x, y and z (as labeled on board). The reported values
// relate to rotation about these axes.  Regarding PHI, the board  has a horizontal
// mounting, with the connector pins going in the left-right axis.  Therefore,
// x = roll, y = pitch and z = yaw.
//
// Note: this function returns delta(degrees) since last read.  The delta is
// computed by noting that each sample represents the average speed (dps) in a
// GYRO_SAMPLE_PERIOD time span.
//

TODO("check for overrun")

void PHI_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {
  
  float pitchDelta = 0;
  float yawDelta = 0;
  float rollDelta = 0;
  
  BYTE status = gyroReadStatus();

  while ((status & GYRO_STATUS_AVAIL) != 0) {

    // something is available

    // LOG_INFO("gyro status = %02Xh", status);

    if ((status & GYRO_STATUS_Y_AVAIL) != 0) {
      // have Y (pitch) data
      pitchDelta += gyroReadDps(GYRO_YL_ADDR) * GYRO_SAMPLE_PERIOD;
    }

    if ((status & GYRO_STATUS_Z_AVAIL) != 0) {
      // have Z (yaw) data
      yawDelta += gyroReadDps(GYRO_ZL_ADDR) * GYRO_SAMPLE_PERIOD;
    }

    if ((status & GYRO_STATUS_X_AVAIL) != 0) {
      // have X (roll) data
      rollDelta += gyroReadDps(GYRO_XL_ADDR) * GYRO_SAMPLE_PERIOD;
    }

    // read status again to see if more in FIFO
    status = gyroReadStatus();
  }

  // copy back
  
  *pPitchDelta = pitchDelta;
  *pYawDelta   = yawDelta;
  *pRollDelta  = rollDelta;
}

INT8 PHI_gyroGetTemp() {
  BYTE txBuff[1];
  BYTE rxBuff[1];

  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_TEMP_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txBuff, 1, rxBuff, 1);

  return rxBuff[0];
}

