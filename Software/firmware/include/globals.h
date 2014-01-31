
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


// init state
EXT BOOL g_initPeriph INIT(FALSE);        // TRUE if hardware peripherals initialized

// startup time (in uSecs)
EXT UINT64 g_startupTime INIT({0});

// system info
EXT struct utsname g_uname;              // can't init this to zero here for some reason

// host IP
EXT UINT32 g_ipAddr INIT(0);

// PhiLink status
EXT PHILINK_STATUS g_phiLinkStatus INIT(OFF);

// HAL
EXT HAL_FUNCS* g_pHal INIT(NULL);


