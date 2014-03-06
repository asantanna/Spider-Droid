//
// OSEPP ADXL345 Accelerometer Module
//

// I2C speed
#define ACCEL_I2C_CLK             ( 400ul*1000ul )

// 80% max for safety
#define ACCEL_SAFE_I2C_CLK        ( (ACCEL_I2C_CLK * 8) / 10 )

//
// NOTE: Comms with the accelerometer occur at (about) 400 kHz, thus a single byte takes:
//
//     TIME/byte = 8 bits/byte * 1/400 mS/bit
//               = 8/400 mS/byte
//               =  1/5  mS/byte
//               =  0.2  mS/byte
//
// TODO: is it 8 bits/byte or are there start/stop bits, blah blah
