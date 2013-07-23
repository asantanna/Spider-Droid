
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
#define BOOL            UINT

#define FALSE           0
#define TRUE            1

#define COUNTOF(arr)    ((int) (sizeof(arr) / sizeof(arr[0])) )

// allocation helpers

#undef  ALLOC           // prevent confusion
#undef  FREE
#undef  ZERO
#undef  ZERO_P

#define PHI_ALLOC(D)    ((D*)phi_allocHelper(sizeof(D)))
#define PHI_ALLOC_N(n)  phi_allocHelper(n)
#define PHI_FREE(p)     free(p)

#define PHI_ZERO(v)     memset(&(v), 0, sizeof(v))
#define PHI_ZERO_P(p,l) memset(p, 0, l)

// time

#define U64_1E6         ((UINT64)1e6)
#define TV_TO_USEC(tv)  ( (((UINT64)tv.tv_sec) * U64_1E6) + ((UINT64)tv.tv_usec) )

// logging

#define LOG_INFO(...)   phi_logMsg("", __VA_ARGS__)
#define LOG_ERR(...)    phi_logMsg("ERROR: ", __VA_ARGS__)
#define LOG_FATAL(...)  { phi_logMsg("FATAL: ", __VA_ARGS__); phi_abortProcess(-1); }

// Linux drivers

#define UART_DRIVER_NAME  "/dev/ttyAMA0"

#define SPI0_DRIVER_NAME  "/dev/spidev0.0"
#define SPI1_DRIVER_NAME  "/dev/spidev0.1"


