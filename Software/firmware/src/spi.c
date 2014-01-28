
//
// Interface to SPI (used for gyroscope)
//
// Raspberry Pi supports SPI speeds in the [3.814 KHz, 125 MHz] range.
// Possible actual speeds are:
//
//   SPI_SPEED = 3.814 KHz * [1, 32768]
//
// SPI driver will round down the requested speed to a possible value.
//
// The two SPI devices, SPI 0 and SPI 1 use the same pins for data.
// The 
//

#include "phi.h"
#include <linux/spi/spidev.h>

// SPI file descriptors (SPI 0 & 1)
static int spiFile[2] = { -1, -1 };

// mutex to sync access to SPI interface
PHI_MUTEX_DECL(mtxSPI);

// SPI 0 SETTINGS - for gyroscope

struct spi_ioc_transfer spi_0_xfer[2] = {
  {
    .delay_usecs =    0,
    .speed_hz =       GYRO_SAFE_SPI_CLK,
    .bits_per_word =  GYRO_SPI_BPW,
  }, {
    .delay_usecs =    0,
    .speed_hz =       GYRO_SAFE_SPI_CLK,
    .bits_per_word =  GYRO_SPI_BPW,
  }
};

// SPI 1 SETTINGS - for ADC

struct spi_ioc_transfer spi_1_xfer[2] = {
  {
    .delay_usecs =    0,
    .speed_hz =       ADC_SAFE_SPI_CLK,
    .bits_per_word =  ADC_SPI_BPW,
  }, {
    .delay_usecs =    0,
    .speed_hz =       ADC_SAFE_SPI_CLK,
    .bits_per_word =  ADC_SPI_BPW,
  }
};

struct spi_ioc_transfer* spi_xfer[2] = {
  spi_0_xfer,
  spi_1_xfer,
};


// internal
int initSpiDriver(char* pDriverName, UINT32 speed, BYTE mode, BYTE bpw);

BOOL spi_init() {

  BOOL rc = TRUE;

  // init SPI 0 for use by gyroscope

  spiFile[0] = initSpiDriver(SPI0_DRIVER_NAME, GYRO_SAFE_SPI_CLK, GYRO_SPI_MODE,  GYRO_SPI_BPW);
  
  if (spiFile[0] < 0) {
    LOG_ERR("spi_init: can't init device driver '%s'", SPI0_DRIVER_NAME);
    goto error_exit;
  }

  // init SPI 1 for use by ADCs

  spiFile[1] = initSpiDriver(SPI1_DRIVER_NAME, ADC_SAFE_SPI_CLK, ADC_SPI_MODE,  ADC_SPI_BPW);

  if (spiFile[1] < 0) {
    LOG_ERR("spi_init: can't init device driver '%s'", SPI1_DRIVER_NAME);
    goto error_exit;
  }

quick_exit:

  return rc;

error_exit:

  rc = FALSE;
  LOG_ERR("spi_init: SPI initialization failed!");
  goto quick_exit;
}

// functions for synchronizing access to SPI
// Note: primitive calls are NOT synchronized, the caller is
// responsible for synchronization

// void spi_lock() {
//   PHI_MUTEX_GET(&mtxSPI);
// }

// void spi_unlock() {
//   PHI_MUTEX_RELEASE(&mtxSPI);
// }

// primitive UART access functions
// NOTE: these are NOT synchronized, caller must use uart_lock()/uart_unlock() if necessary

// Note: the SPI_IOC_MESSAGE(N) macro specifies the number of elements
// in the array of struct spi_ioc_transfer that is passed in to the
// ioctl call below.  All entries in the array are processed without
// CS ever being de-asserted.
//
// Example: In order to send a "hybrid" send/receive sequence without
// de-asserting CS in between, one must pass an array of 2 elements and
// use SPI_IOC_MESSAGE(2).

void spi_send(int spiIdx, BYTE* pTx, int txLen) {

  spi_xfer[spiIdx][0].rx_buf = 0;
  spi_xfer[spiIdx][0].tx_buf = (UINT32) pTx;
  spi_xfer[spiIdx][0].len = (UINT32) txLen;

  if (ioctl(spiFile[spiIdx], SPI_IOC_MESSAGE(1), spi_xfer[spiIdx]) < 0) {
    LOG_ERR("spi_send: ioctl() failed");
  }
}

void spi_receive(int spiIdx, BYTE* pRx, int rxLen) {

  spi_xfer[spiIdx][0].rx_buf = (UINT32) pRx;
  spi_xfer[spiIdx][0].tx_buf = 0;
  spi_xfer[spiIdx][0].len = (UINT32) rxLen;

  if (ioctl(spiFile[spiIdx], SPI_IOC_MESSAGE(1), spi_xfer[spiIdx]) < 0) {
    LOG_ERR("spi_receive: ioctl() failed");
  }
}

void spi_sendreceive(int spiIdx, BYTE* pTx, int txLen, BYTE* pRx, int rxLen) {

  spi_xfer[spiIdx][0].rx_buf = 0;
  spi_xfer[spiIdx][0].tx_buf = (UINT32) pTx;
  spi_xfer[spiIdx][0].len = (UINT32) txLen;
  
  spi_xfer[spiIdx][1].rx_buf = (UINT32) pRx;
  spi_xfer[spiIdx][1].tx_buf = 0;
  spi_xfer[spiIdx][1].len = (UINT32) rxLen;
  
  if (ioctl(spiFile[spiIdx], SPI_IOC_MESSAGE(2), spi_xfer[spiIdx]) < 0) {
    LOG_ERR("spi_sendreceive: ioctl() failed");
  }
}

void spi_exchange(int spiIdx, BYTE* pTx, BYTE* pRx, int dataLen) {
  
  spi_xfer[spiIdx][0].rx_buf = (UINT32) pRx;
  spi_xfer[spiIdx][0].tx_buf = (UINT32) pTx;
  spi_xfer[spiIdx][0].len = (UINT32) dataLen;

  if (ioctl(spiFile[spiIdx], SPI_IOC_MESSAGE(1), spi_xfer[spiIdx]) < 0) {
    LOG_ERR("spi_sendreceive: ioctl() failed");
  }
}

// internal

int initSpiDriver(char* pDriverName, UINT32 speed, BYTE mode, BYTE bpw) {

  int fd = open(pDriverName, O_RDWR);

  if (fd < 0) {
    LOG_ERR("spi_init: can't open SPI device driver '%s'", pDriverName);
    goto error_exit;
  }

  // set SPI mode
  // to read mode: ioctl(fd, SPI_IOC_RD_MODE, &mode);

  if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
    LOG_ERR("spi_init: can't set SPI mode");
    goto error_exit;
  }

  // set SPI bits per word
  // to read bpw : ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bpw;

  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bpw) < 0) {
    LOG_ERR("spi_init: can't set SPI bits per word");
    goto error_exit;
  }

  // set SPI max speed
  // to read speed: ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
    LOG_ERR("spi_init: can't set SPI speed");
    goto error_exit;
  }

  LOG_INFO("spi_init: init'd SPI='%s' to mode=0x%02x, bpw=%d, speed=%d",
    pDriverName, mode, bpw, speed);


quick_exit:

  return fd;

error_exit:

  if (fd > 0) {
    close(fd);
  }

  goto quick_exit;
}
