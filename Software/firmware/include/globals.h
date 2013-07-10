
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

// enable debug msgs to stderr
EXT BOOL g_debugToLog INIT(TRUE);

// saved startup time (in uSecs)
EXT UINT64 g_startupTime INIT({0});

// UART 0 
EXT int g_uart0_fs INIT(-1);

