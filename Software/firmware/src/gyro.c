//
// Interface to PHI's gyroscope
//

#include "phi.h"

#include <math.h>

// internal
void gyroCalibrate();

// FIFO enabled flag
BOOL bUseFifo = TRUE;

// zero calibration values
INT16 zeroX = 0;
INT16 zeroY = 0;
INT16 zeroZ = 0;

// zero thresholding value
INT16 threshX = 0;
INT16 threshY = 0;
INT16 threshZ = 0;

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

BOOL gyroInit(BOOL bEnableFifo) {

  BOOL rc = FALSE;
  BYTE txBuff[2];
  BYTE rxBuff[2];

  // make sure init is atomic in case we use it arbitrarily
  // Note: this is not needed if we just init at the beginning
  // spi_lock(GYRO_SPI_IDX);

  // remember FIFO mode
  bUseFifo = bEnableFifo;

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

  if (bUseFifo == TRUE) {
    
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

  // calibrate gyro to find what zero is
  gyroCalibrate();

  // success
  rc = TRUE;

quick_exit:

  // release SPI
  // spi_unlock(GYRO_SPI_IDX);
  
  return rc;
}

BYTE gyroReadStatus() {
  
  BYTE txBuff[1];
  BYTE rxBuff[1];
  
  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_STATUS_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txBuff, 1, rxBuff, 1);

  return rxBuff[0];
}

BYTE gyroReadFifoSrc() {

  BYTE txBuff[1];
  BYTE rxBuff[1];

  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_FIFO_SRC_ADDR;
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


float gyroReadDps(BYTE lowRegAddr) {

  INT16 raw = gyroReadRawDps(lowRegAddr);

  // LOG_INFO("raw %c dps = %d", 'X' + (char)((lowRegAddr - GYRO_XL_ADDR) / 2), raw);

  // see note above about why we don't substract the zeroRate
  float trueDps = raw * GYRO_MAX_VALUE_MULT;

  return trueDps;
}

void gyroReadFifoSlot_raw(INT16* pX_dps, INT16* pY_dps, INT16* pZ_dps) {
  BYTE txBuff[1];
  BYTE rxBuff[6];

  // read six registers in sequential order to free slot
  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_AUTO_INC | GYRO_XL_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txBuff, 1, rxBuff, 6);

  // copy back
  *pX_dps  = *((INT16 *)rxBuff);
  *pY_dps  = *((INT16 *)(rxBuff+2));
  *pZ_dps  = *((INT16 *)(rxBuff+4));
}

void gyroReadFifoSlot(float* pPitchDps, float* pYawDps, float* pRollDps) {

  // read raw
  INT16 rawX, rawY, rawZ;
  gyroReadFifoSlot_raw(&rawX, &rawY, &rawZ);

  // adjust zero
  rawX -= zeroX;
  rawY -= zeroY;
  rawZ -= zeroZ;

  // apply thresholds
  
  if (abs(rawX) <= threshX) {
    rawX = 0;
  }
  
  if (abs(rawY) <= threshY) {
    rawY = 0;
  }
  
  if (abs(rawZ) <= threshZ) {
    rawZ = 0;
  }
    
  // subtract offset, scale and then copy back
  *pPitchDps = rawY * GYRO_MAX_VALUE_MULT;
  *pYawDps   = rawZ * GYRO_MAX_VALUE_MULT;
  *pRollDps  = rawX * GYRO_MAX_VALUE_MULT;
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

void PHI_gyroGetDeltas_noFifo(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {
  
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

void PHI_gyroGetDeltas_useFifo(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {

  float pitchDelta = 0;
  float yawDelta = 0;
  float rollDelta = 0;

  BYTE fifoSrc = gyroReadFifoSrc();

  // DEBUG
  // int loopCount = 0;
  
  // LOG_INFO("gyro FIFO SRC = %02Xh", fifoSrc);

  while ((fifoSrc & GYRO_FIFO_SRC_EMPTY) == 0) {

    // FIFO not empty
    // read a FIFO slot
    float pitchDps, yawDps, rollDps;
    gyroReadFifoSlot(&pitchDps, &yawDps, &rollDps);

    // accumulate
    pitchDelta += pitchDps * GYRO_SAMPLE_PERIOD;
    yawDelta   += yawDps * GYRO_SAMPLE_PERIOD;
    rollDelta  += rollDps * GYRO_SAMPLE_PERIOD;

    // read src again to see if more in FIFO
    fifoSrc = gyroReadFifoSrc();

    // DEBUG
    // LOG_INFO("gyro FIFO SRC = %02Xh", fifoSrc);
    // loopCount ++;
  }

  // DEBUG
  // LOG_INFO("FIFO loop count=%d", loopCount);

  // copy back

  *pPitchDelta = pitchDelta;
  *pYawDelta   = yawDelta;
  *pRollDelta  = rollDelta;
}

void PHI_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {
  
  bUseFifo ? PHI_gyroGetDeltas_useFifo(pPitchDelta, pYawDelta, pRollDelta)
           : PHI_gyroGetDeltas_noFifo (pPitchDelta, pYawDelta, pRollDelta);
}

INT8 PHI_gyroGetTemp() {
  BYTE txBuff[1];
  BYTE rxBuff[1];

  txBuff[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_TEMP_ADDR;
  spi_sendreceive(GYRO_SPI_IDX, txBuff, 1, rxBuff, 1);

  return rxBuff[0];
}


// gyroscope can have a systematic error and thus zero dps might not
// get reported as 0.  This function should be called with a *stationary*
// gyroscope so it can calculate averages of readings so we can remove
// this error.
//

void gyroCalibrate() {

  // we pretty much have to use the FIFO so we don't lose data.  Therefore,
  // this code aborts with a fatal error if we are not using the FIFO

  if (bUseFifo == FALSE) {
    // we require FIFO now
    LOG_FATAL("gyroCalibrate requires the FIFO to be enabled");
  }

  static const int numSamples = 100;
  int i;

  printf("Calibrating gyroscope with %d samples ... ", numSamples);  

  float sumX = 0;
  float sumY = 0;
  float sumZ = 0;

  float dataX[numSamples];
  float dataY[numSamples];
  float dataZ[numSamples];
  
  for (i = 0 ; i < numSamples ; i++) {

    while ((gyroReadFifoSrc() & GYRO_FIFO_SRC_EMPTY) != 0) {
      // FIFO empty ... sleep for a sample period
      usleep((useconds_t)(GYRO_SAMPLE_PERIOD * 1e6f));
    }

    // read a sample from the FIFO
    INT16 rawX, rawY, rawZ;
    gyroReadFifoSlot_raw(&rawX, &rawY, &rawZ);

    // save data
    dataX[i] = (float) rawX;
    dataY[i] = (float) rawY;
    dataZ[i] = (float) rawZ;

    // also save sum
    sumX += dataX[i];
    sumY += dataY[i];
    sumZ += dataZ[i];
  }

  // calc mean and save as zero value
  zeroX = (INT16) (sumX / numSamples);
  zeroY = (INT16) (sumY / numSamples);
  zeroZ = (INT16) (sumZ / numSamples);

  // now calculate standard deviation of each
  
  sumX = 0;
  sumY = 0;
  sumZ = 0;

  for (i = 0 ; i < numSamples ; i++) {
    sumX += powf(dataX[i] - zeroX,2);
    sumY += powf(dataY[i] - zeroY,2);
    sumZ += powf(dataZ[i] - zeroZ,2);
  }

  float stdX = sqrtf(sumX / numSamples);
  float stdY = sqrtf(sumY / numSamples);
  float stdZ = sqrtf(sumZ / numSamples);

  // datasheet recommends that 3 sigma be used as a threshold
  // for a valid value

  threshX = (INT16) (stdX * 3);
  threshY = (INT16) (stdY * 3);
  threshZ = (INT16) (stdZ * 3);

  // done

  printf("done\n");
  
  printf("Gyro zeros: %.2f,%.2f,%.2f deg\n",
    (double) (zeroX * GYRO_MAX_VALUE_MULT),
    (double) (zeroY * GYRO_MAX_VALUE_MULT),
    (double) (zeroZ * GYRO_MAX_VALUE_MULT));
  
  printf("Gyro threshold: %.2f,%.2f,%.2f deg\n",
    (double) (threshX * GYRO_MAX_VALUE_MULT),
    (double) (threshY * GYRO_MAX_VALUE_MULT),
    (double) (threshZ * GYRO_MAX_VALUE_MULT));
}
