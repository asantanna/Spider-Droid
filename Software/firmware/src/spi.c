
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

// SPI 0 SETTINGS - for gyroscope

struct spi_ioc_transfer spi0_xfer[2] = {
  {
    .delay_usecs = 0,
    .speed_hz = GYRO_SAFE_SPI_CLK,
     .bits_per_word = GYRO_SPI_BPW,
  }, {
    .delay_usecs = 0,
    .speed_hz = GYRO_SAFE_SPI_CLK,
    .bits_per_word = GYRO_SPI_BPW,
  }
};

// internal
int initSpiDriver(char* pDriverName, UINT32 speed, BYTE mode, BYTE bpw);

BOOL spiInit() {

  BOOL rc = TRUE;

  // init SPI 0 for use by gyroscope

  g_spi0_fd = initSpiDriver(SPI0_DRIVER_NAME, GYRO_SAFE_SPI_CLK, GYRO_SPI_MODE,  GYRO_SPI_BPW);
  
  if (g_spi0_fd < 0) {
    LOG_ERR("spiInit: can't init device driver '%s'", SPI0_DRIVER_NAME);
    goto error_exit;
  }

quick_exit:

  return rc;

error_exit:

  rc = FALSE;
  LOG_ERR("spiInit: SPI initialization failed!");
  goto quick_exit;
}

// Note: the SPI_IOC_MESSAGE(N) macro specifies the number of elements
// in the array of struct spi_ioc_transfer that is passed in to the
// ioctl call below.  All entries in the array are processed without
// CS ever being de-asserted.
//
// Example: In order to send a "hybrid" send/receive sequence without
// de-asserting CS in between, one must pass an array of 2 elements and
// use SPI_IOC_MESSAGE(2).

void spi_send(int spiIdx, BYTE* pTx, int txLen) {

  WARN("ignoring spiIdx in spi_send")
  
  spi0_xfer[0].rx_buf = 0;
  spi0_xfer[0].tx_buf = (UINT32) pTx;
  spi0_xfer[0].len = (UINT32) txLen;

  if (ioctl(g_spi0_fd, SPI_IOC_MESSAGE(1), spi0_xfer) < 0) {
    LOG_ERR("spi_send: ioctl() failed");
  }
}

void spi_receive(int spiIdx, BYTE* pRx, int rxLen) {

  WARN("ignoring spiIdx in spi_receive")

  spi0_xfer[0].rx_buf = (UINT32) pRx;
  spi0_xfer[0].tx_buf = 0;
  spi0_xfer[0].len = (UINT32) rxLen;

  if (ioctl(g_spi0_fd, SPI_IOC_MESSAGE(1), spi0_xfer) < 0) {
    LOG_ERR("spi_receive: ioctl() failed");
  }
}

void spi_sendreceive(int spiIdx, BYTE* pTx, int txLen, BYTE* pRx, int rxLen) {

  WARN("ignoring spiIdx in spi_sendreceive")
  
  spi0_xfer[0].rx_buf = 0;
  spi0_xfer[0].tx_buf = (UINT32) pTx;
  spi0_xfer[0].len = (UINT32) txLen;
  
  spi0_xfer[1].rx_buf = (UINT32) pRx;
  spi0_xfer[1].tx_buf = 0;
  spi0_xfer[1].len = (UINT32) rxLen;
  
  if (ioctl(g_spi0_fd, SPI_IOC_MESSAGE(2), spi0_xfer) < 0) {
    LOG_ERR("spi_sendreceive: ioctl() failed");
  }
}

// internal

int initSpiDriver(char* pDriverName, UINT32 speed, BYTE mode, BYTE bpw) {

  int fd = open(pDriverName, O_RDWR);

  if (fd < 0) {
    LOG_ERR("spiInit: can't open SPI device driver '%s'", pDriverName);
    goto error_exit;
  }

  // set SPI mode
  // to read mode: ioctl(fd, SPI_IOC_RD_MODE, &mode);

  if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
    LOG_ERR("spiInit: can't set SPI mode");
    goto error_exit;
  }

  // set SPI bits per word
  // to read bpw : ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bpw;

  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bpw) < 0) {
    LOG_ERR("spiInit: can't set SPI bits per word");
    goto error_exit;
  }

  // set SPI max speed
  // to read speed: ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
    LOG_ERR("spiInit: can't set SPI speed");
    goto error_exit;
  }

  LOG_INFO("spiInit: init'd SPI='%s' to mode=0x%02x, bpw=%d, speed=%d",
    pDriverName, mode, bpw, speed);


quick_exit:

  return fd;

error_exit:

  if (fd > 0) {
    close(fd);
  }

  goto quick_exit;
}
