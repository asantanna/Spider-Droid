//
// OSEPP ADXL345 Accelerometer Module
//

// I2C speed
#define ACCEL_I2C_CLK             ( 400ul*1000ul )

// 80% max for safety
#define GYRO_SAFE_SPI_CLK         ( 8ul*1000ul*1000ul)

//
// NOTE: Comms with the accelerometer occur at 400 kHz, thus a single byte takes:
//
//     TIME/byte = 8 bits/byte * 1/8 uS/bit = 1 uS
//
