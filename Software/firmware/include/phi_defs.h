
//
// spiderBot common types
//

#define PHI_VERSION     "0.1 dev"

// logging
#define LOGFILE_NAME    "/var/log/phi.log"
#define VERBOSE_LOG     1

// signed ints
#define INT8            char
#define INT16           short
#define INT32           long
#define INT64           long long

// unsigned ints
#define UINT            unsigned int
#define UINT8           unsigned char
#define UINT16          unsigned short
#define UINT32          unsigned long
#define UINT64          unsigned long long

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
#define CAM_WIDTH               160
#define CAM_HEIGHT              120
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
#define HWPUMP_LOOPS_PER_SEC        100
#define HW_PUMP_LOOP_PERIOD_USEC    ((INT32)(1e6 / HWPUMP_LOOPS_PER_SEC))

// performance monitoring

#define PERFLOG_HWPUMP_ELEM_1SEC      HWPUMP_LOOPS_PER_SEC
#define PERFLOG_HWPUMP_ELEM_5SEC      (5 * PERFLOG_HWPUMP_ELEM_1SEC)
#define PERFLOG_HWPUMP_ELEM_15SEC     (15 * PERFLOG_HWPUMP_ELEM_1SEC)
#define PERFLOG_HWPUMP_NUM_ELEM       PERFLOG_HWPUMP_ELEM_15SEC
#define PERFLOG_HWPUMP_EPOCH          15

