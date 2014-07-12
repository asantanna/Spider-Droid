
//
// spiderBot common types
//

#define PHI_VERSION     "0.1 dev"

//
// GENERIC DEFS
//
// Note: use GNU types to make sure things are what
// we expect.
//

// signed ints
#define INT8            int8_t
#define INT16           int16_t
#define INT32           int32_t
#define INT64           int64_t

// unsigned ints
#define UINT8           uint8_t
#define UINT16          uint16_t
#define UINT32          uint32_t
#define UINT64          uint64_t


// common data aliases

#define BYTE            UINT8
#define WORD            UINT16
#define DWORD           UINT32
#define BOOL            UINT8

#define FALSE           0
#define TRUE            1

// math macros
#define COUNTOF(arr)    ((int) (sizeof(arr) / sizeof(arr[0])) )
#define MIN(a,b)        ( ((a) < (b)) ? (a) : (b) )
#define MAX(a,b)        ( ((a) > (b)) ? (a) : (b) )

// allocation helpers

#undef  ALLOC           // prevent confusion
#undef  FREE
#undef  ZERO
#undef  ZERO_P

#define PHI_ALLOC(D)    ((D*)allocHelper(sizeof(D)))
#define PHI_ALLOC_N(n)  allocHelper(n)
#define PHI_FREE(p)     {free(p); p = NULL; }

#define PHI_ZERO(v)     memset(&(v), 0, sizeof(v))
#define PHI_ZERO_P(p,l) memset(p, 0, l)

// preprocessor

#define BASH_BLACK      "30"
#define BASH_RED        "31"
#define BASH_GREEN      "32"
#define BASH_YELLOW     "33"
#define BASH_BLUE       "34"
#define BASH_MAGENTA    "35"
#define BASH_CYAN       "36"
#define BASH_WHITE      "37"

#define BASH_START_COLOR(c)   "\e[1;" c "m"
#define BASH_STOP_COLOR       "\e[0m"

#define DO_PRAGMA(x)    _Pragma (#x)
#define INFO(m)         DO_PRAGMA(message (BASH_START_COLOR(BASH_GREEN)  "INFO - " m BASH_STOP_COLOR))
#define TODO(m)         DO_PRAGMA(message (BASH_START_COLOR(BASH_CYAN)  "TODO - " m BASH_STOP_COLOR))
#define WARN(m)         DO_PRAGMA(message (BASH_START_COLOR(BASH_YELLOW)    "WARNING - " m BASH_STOP_COLOR))
#define HACK(m)         DO_PRAGMA(message (BASH_START_COLOR(BASH_RED) "HACK - " m BASH_STOP_COLOR))

// time
#define U64_1E6         ((UINT64)1e6)
#define TV_TO_USEC(tv)  ( (((UINT64)tv.tv_sec) * U64_1E6) + ((UINT64)tv.tv_usec) )

// logging
#define LOG_INFO(...)   logMsg("", __VA_ARGS__)
#define LOG_ERR(...)    logMsg("ERROR: ", __VA_ARGS__)
#define LOG_FATAL(...)  { printf("FATAL: "); printf(__VA_ARGS__); printf("\n"); logMsg("FATAL: ", __VA_ARGS__); abortProcess(-1); }

// stringifying helpers
#define STR(s)                #s              /* convert to string */
#define STR_MACRO(m)          STR(m)          /* convert to string (version for use inside a macro - requires two steps */
#define Q(s)                  "\"" #s "\""    /* convert to quoted string */

//
// PHI SPECIFIC DEFS
//

// logging
#define LOGFILE_NAME    "/var/log/phi.log"

#define LOG_LEVEL                 (LOG_LEVEL_HTTP | LOG_LEVEL_JSON)
#define LOG_LEVEL_HTTP            0x0001
#define LOG_LEVEL_HTTP_VERBOSE    0x0002
#define LOG_LEVEL_JSON            0x0004
#define LOG_LEVEL_JSON_VERBOSE    0x0008

// HTTP/JSON buffers
#define HTTP_BUFFSIZE       (16*1024)      // max size of HTTP request/reply
#define JSON_TMP_BUFFSIZE   (4*1024)       // max size of single JSON reply

// Linux drivers
#define UART_DRIVER_NAME  "/dev/ttyAMA0"
#define SPI0_DRIVER_NAME  "/dev/spidev0.0"
#define SPI1_DRIVER_NAME  "/dev/spidev0.1"

// LED colors

typedef enum {
    OFF,
    RED,
    FLASH_RED,
    YELLOW,
    FLASH_YELLOW,
    GREEN,
    FLASH_GREEN
    
} PHI_LED_COLOR;

// cam info (move to cam file when it exists)
#define CAM_WIDTH               8 //160
#define CAM_HEIGHT              8 // 120
#define CAM_NUM_BYTES           (CAM_WIDTH * CAM_HEIGHT)

// PhiLink
#define DEF_PHILINK_PORT  1122

// SPI indexes
#define GYRO_SPI_IDX      0         // gyro at idx 0
#define ADC_SPI_IDX       1         // ADCs at idx 1

// mutex macros

#define PHI_MUTEX_VAR_TYPE          pthread_mutex_t
#define PHI_MUTEX_INITVAL           PTHREAD_MUTEX_INITIALIZER
#define PHI_MUTEX_DECL(name)        PHI_MUTEX_VAR_TYPE name
#define PHI_MUTEX_DECL_INIT(name)   PHI_MUTEX_DECL(name) = PHI_MUTEX_INITVAL
#define PHI_MUTEX_INIT(pMtx)        pthread_mutex_init(pMtx, NULL)
#define PHI_MUTEX_GET(pMtx)         pthread_mutex_lock(pMtx)
#define PHI_MUTEX_RELEASE(pMtx)     pthread_mutex_unlock(pMtx)
#define PHI_MUTEX_TRYGET(pMtx)      pthread_mutex_trylock(pMtx)   // ret 0 if get successful, ret != if failed

// hardware pumping rate
#define HWPUMP_LOOPS_PER_SEC        30
#define HW_PUMP_LOOP_PERIOD_USEC    ((INT32)(1e6 / HWPUMP_LOOPS_PER_SEC))

// performance monitoring

#define PERFLOG_HWPUMP_ELEM_1SEC      HWPUMP_LOOPS_PER_SEC
#define PERFLOG_HWPUMP_ELEM_5SEC      (5 * PERFLOG_HWPUMP_ELEM_1SEC)
#define PERFLOG_HWPUMP_ELEM_15SEC     (15 * PERFLOG_HWPUMP_ELEM_1SEC)
#define PERFLOG_HWPUMP_NUM_ELEM       PERFLOG_HWPUMP_ELEM_15SEC
#define PERFLOG_HWPUMP_EPOCH          15

#define PERFLOG_PHILINK_ELEM_1SEC     50
#define PERFLOG_PHILINK_ELEM_5SEC     (5  * PERFLOG_PHILINK_ELEM_1SEC)
#define PERFLOG_PHILINK_ELEM_15SEC    (15 * PERFLOG_PHILINK_ELEM_1SEC)
#define PERFLOG_PHILINK_NUM_ELEM      PERFLOG_PHILINK_ELEM_15SEC
#define PERFLOG_PHILINK_EPOCH         15

//
// GPIO setup macros.
//
// NOTE: ALWAYS use SET_GPIO_TO_INPUT(g) before using SET_GPIO_TO_OUTPUT(g) or SET_GPIO_ALT(g,a)
//       (see gpio.c for examples of how to use)
//

// BCM2835 pins available through the REV1 Raspberry Pi header
#define REV1_PINS {0, 1, 4, 7, 8, 9, 10, 11, 14, 15, 17, 18, 21, 22, 23, 24, 25}

// BCM2835 pins available through the REV2 Raspberry Pi header
#define REV2_PINS {2, 3, 4, 7, 8, 9, 10, 11, 14, 15, 17, 18, 22, 23, 24, 25, 27}

// GPIO pins used by PHI
#define PHI_GPIO_SPI1_SEL        4
#define PHI_GPIO_ADC0_SEL       17
#define PHI_GPIO_ADC1_SEL       22
#define PHI_GPIO_ADC2_SEL       27

// pin mode control
#define SET_GPIO_TO_INPUT(g)    *(g_pGpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define SET_GPIO_TO_OUTPUT(g)   *(g_pGpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a)       *(g_pGpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

// bit read
#define GPIO_READ_ALL       *(g_pGpio+(GPLEV0_OFFSET/4))                              // reads all bits
#define GPIO_ISHIGH(pin)    ((((GPIO_READ_ALL) & (1 << (pin))) != 0) ? TRUE : FALSE)  // TRUE if bit is HIGH
#define GPIO_ISLOW(pin)     (!(GPIO_ISHIGH(pin)))                                     // TRUE if bit is LOW

// bit write
#define GPIO_SET_ALL(v)     {*(g_pGpio+(GPSET0_OFFSET/4)) = (v);}   // SETS bits which are 1 ignores bits which are 0
#define GPIO_CLEAR_ALL(v)   {*(g_pGpio+(GPCLR0_OFFSET/4)) = (v);}   // CLEARS bits which are 1 ignores bits which are 0
#define GPIO_SET(pin)       GPIO_SET_ALL(1<<(pin))
#define GPIO_CLEAR(pin)     GPIO_CLEAR_ALL(1<<(pin))


