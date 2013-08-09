//
// Parallax L3G4200D Gyroscope Module
//

// max SPI speed 10MHz
#define GYRO_MAX_SPI_CLK          (10ul*1000ul*1000ul)

// 80% max for safety
#define GYRO_SAFE_SPI_CLK         ( 8ul*1000ul*1000ul)

// Gyro SPI mode
//
//   1) SCLK idles HIGH (CPOL = 1)
//   2) sampling occurs on even edges (CPHA = 1)
//   3) MSB (bit) sent out first
//   4) bytes are sent out little-endian, LSB (byte) first
//   4) CS is active low.
//

#define GYRO_SPI_MODE             (SPI_CPHA | SPI_CPOL)

// 8 bits per word
#define GYRO_SPI_BPW
8

// multipliers for raw dps values
#define GYRO_250DPS_MULT          0.00875f
#define GYRO_500DPS_MULT          0.0175f
#define GYRO_2000DPS_MULT         0.07f

//
// Command format
//

// address byte

#define GYRO_ADDR_READ            0x80
#define GYRO_ADDR_WRITE           0

#define GYRO_ADDR_AUTO_INC        0x40
#define GYRO_ADDR_NO_INC          0

#define GYRO_ADDR_MASK            0x3F

//
// Control registers
//

// WHO_AM_I

#define GYRO_WHOAMI_ADDR          0x0F
#define GYRO_WHOAMI_REPLY         0xD3

// CTRL_REG1

#define GYRO_CR1_ADDR             0x20

#define GYRO_CR1_DR_MASK          0xC0

#define GYRO_CR1_DR_100HZ         0x00  /* output data rate */
#define GYRO_CR1_DR_200HZ         0x40
#define GYRO_CR1_DR_400HZ         0x80
#define GYRO_CR1_DR_800HZ         0xC0

#define GYRO_CR1_BW_MASK          0x30  /* cutoff freqs - see datasheet */
#define GYRO_CR1_BW_CO_1          0x00
#define GYRO_CR1_BW_CO_2          0x10
#define GYRO_CR1_BW_CO_3          0x20
#define GYRO_CR1_BW_CO_4          0x30

#define GYRO_CR1_MODE_PWRDOWN     0
#define GYRO_CR1_MODE_NORMAL      0x08
#define GYRO_CR1_Z_EN             0x04
#define GYRO_CR1_Y_EN             0x02
#define GYRO_CR1_X_EN             0x01

// CTRL_REG2

#define GYRO_CR2_ADDR             0x21

#define GYRO_CR2_RSVD_MASK        0xC0  /* these bits must not be changed */
#define GYRO_CR2_HPM_MASK         0x30  /* see datasheet */
#define GYRO_CR2_HPCF_MASK        0x0F

// CTRL_REG3

#define GYRO_CR3_ADDR             0x22

#define GYRO_CR3_INT1_EN          0x80
#define GYRO_CR3_INT1_BOOT_EN     0x40
#define GYRO_CR3_INT1_LOW         0x20
#define GYRO_CR3_OPEN_DRAIN       0x10
#define GYRO_CR3_INT2_DRDY_EN     0x08
#define GYRO_CR3_INT2_FWI_EN      0x04
#define GYRO_CR3_INT2_OI_EN       0x02
#define GYRO_CR3_INT2_FEMPTY_EN   0x01

// CTRL_REG4


#define GYRO_CR4_ADDR             0x23

#define GYRO_CR4_BDU_EN           0x80
#define GYRO_CR4_BIG_ENDIAN       0x40
#define GYRO_CR4_LITTLE_ENDIAN    0

#define GYRO_CR4_FS_MASK          0x30
#define GYRO_CR4_FS_250DPS        0x00
#define GYRO_CR4_FS_500DPS        0x10
#define GYRO_CR4_FS_2000DPS       0x20

#define GYRO_CR4_STEST_MASK       0x06
#define GYRO_CR4_SPI_3WIRE        0x01

// CTRL_REG5

#define GYRO_CR5_ADDR             0x24

#define GYRO_CR5_REBOOT           0x80
#define GYRO_CR5_FIFO_EN          0x40
#define GYRO_CR5_HP_EN            0x10
#define GYRO_CR5_INT1_SEL_MASK    0x0C
#define GYRO_CR5_OUT_SEL_MASK     0x03

// REFERENCE/DATACAPTURE

#define GYRO_REF_ADDR             0x25

// OUT_TEMP

#define GYRO_TEMP_ADDR            0x26

// STATUS_REG

#define GYRO_STATUS_ADDR          0x27

#define GYRO_STATUS_OVER          0x80
#define GYRO_STATUS_Z_OVER        0x40
#define GYRO_STATUS_Y_OVER        0x20
#define GYRO_STATUS_X_OVER        0x10
#define GYRO_STATUS_AVAIL         0x08
#define GYRO_STATUS_Z_AVAIL       0x04
#define GYRO_STATUS_Y_AVAIL       0x02
#define GYRO_STATUS_X_AVAIL       0x01

// OUT_X_L, OUT_X_H

#define GYRO_XL_ADDR              0x28
#define GYRO_XH_ADDR              0x29

// OUT_Y_L, OUT_Y_H

#define GYRO_YL_ADDR              0x2A
#define GYRO_YH_ADDR              0x2B

// OUT_Z_L, OUT_Z_H

#define GYRO_ZL_ADDR              0x2C
#define GYRO_ZH_ADDR              0x2D

// FIFO_CTRL_REG

#define GYRO_FIFO_ADDR            0x2E

#define GYRO_FIFO_MODE_BYPASS     0x00
#define GYRO_FIFO_MODE_FIFO       0x20
#define GYRO_FIFO_MODE_STREAM     0x40
#define GYRO_FIFO_MODE_STF        0x60
#define GYRO_FIFO_MODE_BTS        0x80
#define GYRO_FIFO_WTM_MASK        0x1F

// FIFO_CTRL_REG

#define GYRO_SRC_ADDR             0x2F

// INT1_CFG

#define GYRO_RSRC_ADDR            0x2F
