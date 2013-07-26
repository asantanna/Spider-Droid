//
// Interface to PHI's gyroscope
//

#include "phi.h"

BOOL gyroInit() {
  
  BYTE data[2];

  // make sure gyro is there
  data[0] = GYRO_ADDR_READ | GYRO_ADDR_NO_INC | GYRO_WHOAMI_ADDR;
  spi0Send(data, 1);
  spi0Receive(data, 1);
  
  if (data[0] != GYRO_WHOAMI_REPLY) {
    // bad (or no) reply
    LOG_ERR("bad (or no) reply from gyroscope, reply=%02x", data[0]);
    return FALSE;
  }

  // HACK - leave CR2 at default values
  
  // CR3 - default values OK
  
  // CR4
  
  data[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR4_ADDR;
  data[1] = GYRO_CR4_BDU_EN | GYRO_CR4_BIG_ENDIAN;
  spi0Send(data, 2);

  // CR5 - default values OK

  // REFERENCE - not used

  // INT1_THS - not used

  // INT1_DURATION - not used

  // INT1_CFG - default values OK (ints disabled)

  // CR1
  data[0] = GYRO_ADDR_WRITE | GYRO_ADDR_NO_INC | GYRO_CR1_ADDR;
  data[1] = GYRO_CR1_DR_100HZ | GYRO_CR1_BW_CO_1 | GYRO_CR1_X_EN | GYRO_CR1_Y_EN | GYRO_CR1_Z_EN;
  spi0Send(data, 2);

  // success
  return TRUE;
}