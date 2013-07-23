
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

// SPI settings for gyroscope module   
#define GYRO_SPI_CLK    (8ul*1000ul*1000ul)
#define GYRO_SPI_MODE   (SPI_CPHA | SPI_CPOL | SPI_LSB_FIRST)
#define GYRO_SPI_BPW     7

// SPI 0 SETTINGS (for gyroscope module)
   
UINT32 spi0_speed = GYRO_SPI_CLK;
BYTE spi0_mode = GYRO_SPI_MODE;
BYTE spi0_bpw = GYRO_SPI_BPW;

struct spi_ioc_transfer spi0_xfer = {
  .tx_buf = 0,
  .rx_buf = 0,
  .len = 0,
  .delay_usecs = 0,
  .speed_hz = GYRO_SPI_CLK,
  .bits_per_word = GYRO_SPI_BPW,
};

/* SPI 1 SETTINGS

UINT32 spi1_speed = 8ul*1000ul*1000ul;
BYTE spi1_mode = SPI_CPHA | SPI_CPOL | SPI_LSB_FIRST;
BYTE spi1_bpw = 7;

struct spi_ioc_transfer spi1_xfer = {
  .tx_buf = 0,
  .rx_buf = 0,
  .len = 0,
  .delay_usecs = 0,
  .speed_hz = spi1_speed,
  .bits_per_word = spi1_bpw,
}; */


BOOL spiInit() {

  BOOL rc = TRUE;

  // hack - refactor this when we need to user SPI 1 for something

  // 
  // Init SPI 0 - Gyroscope
  //

  g_spi0_fs = open(SPI0_DRIVER_NAME, O_RDWR);

  if (g_spi0_fs < 0) {
    LOG_ERR("spiInit: can't open SPI 0 device driver");
    goto error_exit;
  }

  // set SPI 0 mode
  // to read mode: ioctl(g_spi0_fs, SPI_IOC_RD_MODE, &spi0_mode);

  if (ioctl(g_spi0_fs, SPI_IOC_WR_MODE, &spi0_mode) < 0) {
    LOG_ERR("spiInit: can't set SPI 0 mode");
    goto error_exit;
  }

  // set SPI bits per word
  // to read bpw : ioctl(g_spi0_fs, SPI_IOC_RD_BITS_PER_WORD, &spi0_bpw;

  if (ioctl(g_spi0_fs, SPI_IOC_WR_BITS_PER_WORD, &spi0_bpw) < 0) {
    LOG_ERR("spiInit: can't set SPI 0 bits per word");
    goto error_exit;
  }

  // set SPI max speed
  // to read speed: ioctl(g_spi0_fs, SPI_IOC_RD_MAX_SPEED_HZ, &spi0_speed);

  if (ioctl(g_spi0_fs, SPI_IOC_WR_MAX_SPEED_HZ, &spi0_speed) < 0) {
    LOG_ERR("spiInit: can't set SPI 0 speed");
    goto error_exit;
  }

  LOG_INFO("spiInit: init'd SPI 0 to mode=0x%x, bpw=%d, speed=%d", spi0_mode, spi0_bpw, spi0_speed);
  
quick_exit:

  return rc;

error_exit:

  rc = FALSE;
  LOG_ERR("spiInit: SPI 0 initialization failed!");
  goto quick_exit;
}

void spiTransfer(int spiID, BYTE* rxBuff, BYTE* txBuff, UINT32 dataLen) {

  // hack - refactor this when we need to user SPI 1 for something
  // spiID is ignored

  spi0_xfer.rx_buf = (UINT32) rxBuff;
  spi0_xfer.tx_buf = (UINT32) txBuff;
  spi0_xfer.len = dataLen;
  
  if (ioctl(g_spi0_fs, SPI_IOC_MESSAGE(1), &spi0_xfer) < 0) {
    LOG_ERR("spiTransfer: ioctl() failed");
  }
}

void spiSend(BYTE* pData, int dataLen) {
  spiTransfer(0, NULL, pData, dataLen);
}

void spiReceive(void* pBuff, int buffLen) {
  spiTransfer(0, pBuff, NULL, buffLen);
}

