//
// Microchip MCP3008
// 8-Channel 10-bit A/D Converter with SPI Interface
//

// SPI clk is used to time the sampling.  Sample time
// is 1.5 * SPI clk period.
//
// There is a lower limit to make sure the sampling
// capacitor retains its charge during conversion.
// There is also an upper limit to allow enough time
// for the capacitor to charge.


#define ADC_MIN_SPI_CLK         (10ul   * 1000ul)
#define ADC_MAX_SPI_CLK         (1350ul * 1000ul)

#define ADC_SAFE_SPI_CLK        (1000ul * 1000ul)

//
// NOTE: Exchanges with the ADC consist of 3 bytes @ 1 MHz, thus an exchange takes:
//
//     TIME = 3 bytes * 8 bits/byte * 1 uS/bit = 24 uS
//

// ADC SPI mode
//
//   1) SCLK idles LOW (CPOL = 0)
//   2) sampling occurs on odd edges (CPHA = 0)
//   3) MSB (bit) sent out first
//   4) bytes are sent out little-endian, LSB (byte) first
//   4) CS is active low.
//

// #define ADC_SPI_MODE            (0|0)
#define ADC_SPI_MODE            (SPI_CPHA | SPI_CPOL)

// 8 bits per word
#define ADC_SPI_BPW             8

// macro to convert ADC values to volts
#define ADC_RAW_TO_VOLTS(val, v_ref)      ( ((float)(val)) * ((float)(v_ref)) / 1024.0f )

//
// Command format
//
// Note: SPI must be full duplex.  There is an exchange of 3 bytes.
// The first is purely outgoing, the second is part outgoing, part
// incoming and the third is purely incoming.
//

// byte 1 (outgoing)

#define ADC_CMD1_START          0x01      // this is the "start bit"

// byte 2 (high nibble out/low nibble in)

#define ADC_CMD2_SINGLE         0x80      // use single ended inputs (8x)
#define ADC_CMD2_DIFF           0x00      // use differential inputs (4x)
#define ADC_CMD2_ADDR_MASK      0x70      // address mask (bits = [0,7])

#define ADC_DATA2_NOT_USED_MASK 0x08      // this bit is not used
#define ADC_DATA2_NULL_MASK     0x04      // this bit will be zero
#define ADC_DATA2_MASK          0x03      // these bits are the result's high bits

// byte 3 (incoming)

#define ADC_DATA3_MASK          0xFF      // these bits are the result's low bits

