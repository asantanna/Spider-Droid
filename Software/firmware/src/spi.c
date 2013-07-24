
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

// SPI 0 SETTINGS
   

struct spi_ioc_transfer spi0_xfer = {
  .tx_buf = 0,
  .rx_buf = 0,
  .len = 0,
  .delay_usecs = 0,
  .speed_hz = GYRO_SPI_CLK,
  .bits_per_word = GYRO_SPI_BPW,
};

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
}


BOOL spiInit() {

  BOOL rc = TRUE;

  // init SPI 0 for use by gyroscope

  g_spi0_fd = initSpiDriver(SPI0_DRIVER_NAME, GYRO_SPI_CLK, GYRO_SPI_MODE,  GYRO_SPI_BPW);
  
  if (g_spi0_fd < 0) {
    LOG_ERR("spiInit: can't init device driver '%s'", pDriverName);
    goto error_exit;
  }

quick_exit:

  return rc;

error_exit:

  rc = FALSE;
  LOG_ERR("spiInit: SPI initialization failed!");
  goto quick_exit;
}

void spiTransfer(int spiID, BYTE* rxBuff, BYTE* txBuff, UINT32 dataLen) {

  spi0_xfer.rx_buf = (UINT32) rxBuff;
  spi0_xfer.tx_buf = (UINT32) txBuff;
  spi0_xfer.len = dataLen;
  
  if (ioctl(g_spi0_fd, SPI_IOC_MESSAGE(1), &spi0_xfer) < 0) {
    LOG_ERR("spiTransfer: ioctl() failed");
  }
  
}

void spi0Send(BYTE* pData, int dataLen) {
  spiTransfer(0, NULL, pData, dataLen);
}

void spi0Receive(void* pBuff, int buffLen) {
  spiTransfer(0, pBuff, NULL, buffLen);
}

