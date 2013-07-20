
//
// Global variables for spiderDroid (Phi)
//

#ifdef GLOBAL_INIT
#define INIT(v)   = v
#define EXT       // nothing
#else
#define INIT(v)   // nothing
#define EXT       extern
#endif

//
// init state
//

EXT BOOL g_initPeriph INIT(FALSE);        // TRUE if UART peripherals initialized

// startup time (in uSecs)
EXT UINT64 g_startupTime INIT({0});

// host IP
EXT UINT32 g_ipAddr INIT(0);


// UART 0 
EXT int g_uart0_fs INIT(-1);

