//
// Parallax L3G4200D Gyroscope Module
//

// max SPI speed 10MHz
#define GYRO_MAX_SPI_CLK          (10ul*1000ul*1000ul)

// use 8MHz for safety
#define GYRO_SPI_CLK              ( 8ul*1000ul*1000ul)

// SPI mode
//
//   1) SCLK idles HIGH (CPOL = 1)
//   2) sampling occurs on even edges (CPHA = 1)
//   3) MSB sent out first
//   4) CS is active low.
//

#define GYRO_SPI_MODE             (SPI_CPHA | SPI_CPOL)

// 8 bits per word
#define GYRO_SPI_BPW              8

//
// Command format
//

// address byte

#define GYRO_ADDR_READ            0x80
#define GYRO_ADDR_WRITE           0

#define GYRO_ADDR_AUTO_INC        0x40
#define GYRO_ADDR_NO_INC          0

#define GYRO_ADDR_MASK            0x3F






