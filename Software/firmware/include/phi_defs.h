
//
// spiderBot common types
//

#define PHI_VERSION     "0.1 dev"
#define LOGFILE_NAME    "/var/log/phi.log"

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

// allocation helpers

#define ALLOC(D)        ((D*)phi_allocHelper(sizeof(D)))

// time

#define U64_1E6         ((UINT64)1e6)
#define TV_TO_USEC(tv)  ( (((UINT64)tv.tv_sec) * U64_1E6) + ((UINT64)tv.tv_usec) )

// logging

#define LOG_INFO(...)   phi_logMsg("", __VA_ARGS__)
#define LOG_ERR(...)    phi_logMsg("ERROR: ", __VA_ARGS__)





