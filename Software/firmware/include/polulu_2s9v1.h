//
// Polulu 2s9v1 Dual Serial PWM motor controller
//

// default baud rate (using Linux defines)
#define MC_DEF_BAUD   B38400

// Polulu Protocol (8N1, LSB first, TTL level signals)
//
// Note: compact protocol not documented here
//
// Note 2: controller RX and ERR lines can be wired together but
//         TX lines cannot.  Therefore we do not use the TX line
//         and ignore error codes.
//
// Format:
//
// byte 0: CMD_SIGN
// byte 1: DEVICE_ID
// byte 2: CMD_CODE
// byte n: data bytes
//

#define MC_CMD_SIGN                 0xAA
#define MC_DEFAULT_DEVICE_NUM       9

#define MC_CMD_GET_VERSION          1         // not used
#define MC_CMD_GET_ERR_BYTE         2         // not used
#define MC_CMD_GET_CONF             3         // not used
#define MC_CMD_SET_CONF             4         // configure
#define MC_CMD_UNKNOWN              5         // not doc
#define MC_CMD_COAST_M0             6         // no resist motor 0
#define MC_CMD_COAST_M1             7         // no resist motor 1

#define MC_CMD_FWD_M0               8         // set M0 speed (forward)
#define MC_CMD_FWD_8BIT_M0          9         // set M0 8-bit speed+128 (forward)
#define MC_CMD_BCK_M0             0xA         // set M0 speed (backward)
#define MC_CMD_BCK_8BIT_M0        0xB         // set M0 8-bit speed+128 (backward)

#define MC_CMD_FWD_M1             0xC         // set M1 speed (forward)
#define MC_CMD_FWD_8BIT_M1        0xD         // set M1 8-bit speed+128 (forward)
#define MC_CMD_BCK_M1             0xE         // set M1 speed (backward)
#define MC_CMD_BCK_8BIT_M1        0xF         // set M1 8-bit speed+128 (backward)

// CMD_SET_CONF - write conf to non-volatile mem
//
// *** NOTE: this command takes 4 mS to execute and we must wait for this
// *** time to elapse after issuing the command.
//
// data bytes:
// byte 3:  PARAM_NUM     (parameter to set)
// byte 4:  PARAM_VAL     (param value)
// byte 5:  END_VAL_0     (0x55 : these are here to prevent accidental issuing
//                        of this command)
// byte 6:  END_VAL_1     (0x2A)
//
// Param indexes:
// param 0: DEVICE_ID    (controller device ID)
// param 1: PWM_MODE     (pwm frequency and resolution - DEFAULT = 0)
//         0 = high-frequency, 7-bit mode (PWM frequency of 31.5 kHz, which is ultrasonic)
//         1 = high-frequency, 8-bit mode (PWM frequency of 15.7 kHz)
//         2 = low-frequency, 7-bit mode (PWM frequency of 7.8 kHz)
//         3 = low-frequency, 8-bit mode (PWM frequency of 3.9 kHz)
// param 2: ERR_MODE  (1=stop motors if error)
// param 3: TIMEOUT  (cause error if serial timeout)
//

#define MC_SCONF_WAIT_MS            4         // command takes 4 mS to complete - must wait

#define MC_SCONF_END_VAL_0          0x55      // end value 0
#define MC_SCONF_END_VAL_1          0x2A      // end value 1

#define MC_SCONF_PARAM_DEVICE_ID    0         // set motor speed
#define MC_SCONF_PARAM_PWM_MODE     1         // set PWM mode
#define MC_SCONF_PARAM_ERR_MODE     2         // set error mode
#define MC_SCONF_PARAM_TIMEOUT      3         // set serial timeout

// PARAM_PWM_MODE
#define MC_SCONF_SPD_7BIT           0         // use 7 bit motor speed - max is 0x7f
#define MC_SCONF_SPD_8BIT           1         // use 8 bit motor speed - max is 0xff
#define MC_SCONF_HIGH_FREQ          0         // higher pitched noise - more power consumption
#define MC_SCONF_LOW_FREQ           2         // lower/louder pitched noise - less power consumption

// PARAM_ERR_MODE
#define MC_SCONF_IGNORE_ERR         0         // ignore errors - keep running
#define MC_SCONF_STOP_ON_ERR        1         // stop motors if error or timeout

// PARAM_TIMEOUT
#define MC_SCONF_TIMEOUT_DISABLE    0         // disable error on timeout
#define MC_SCONF_TO_MULT_MASK       0x0F      // low 4 bits are "multiplier"
#define MC_SCONF_TO_SHIFT_MASK      0x70      // next 3 bits are "shift"
// N = (byte & MULT_MASK) << ((byte & SHIFT_MASK) >> 4)
// timeout = N * 262 mS
// example: N = 01011110 (shift=101=5,  mult=1110=14)
//          N = 14 << 5 = 1 1100 0000 = 0x1C0 = 448
//          timeout = 448 * 262 mS = 117.376 seconds





