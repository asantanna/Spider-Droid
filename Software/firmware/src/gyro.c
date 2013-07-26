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

  // HACK - leave CR2 at default values
  
  // CR3 - default values OK
  
  // CR4
  //
  // Note: bits are sent out MSB but bytes are sent out LSB (little endian)
  
  txData[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR4_ADDR;
  txData[1] = GYRO_CR4_BDU_EN | GYRO_CR4_LITTLE_ENDIAN;
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