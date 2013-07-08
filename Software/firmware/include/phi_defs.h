
//
// spiderBot common types
//

#define PHI_VERSION     "0.1 dev"

// signed ints
#define INT8            char
#define INT16           short
#define INT32           long

// unsigned ints
#define UINT            unsigned int
#define UINT8           unsigned char
#define UINT16          unsigned short
#define UINT32          unsigned long

// common data aliases

#define BYTE            UINT8
#define WORD            UINT16
#define DWORD           UINT32
#define BOOL            UINT

#define FALSE           0
#define TRUE            1

// allocation helpers

#define ALLOC(D)        ((D*)phi_allocHelper(sizeof(D)))



