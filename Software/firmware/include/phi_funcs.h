//
// PHI function declarations
//

//
// HELPERS
//

// clears mem, debug
void* phi_allocHelper(int size);

//
// MISC
//

char* initPeripherals();

void phi_abortProcess(int rc);
void phi_abortWithMsg(const char* msg);

UINT64 phi_upTime();
double phi_rand();

//
// non-ANSI
//

char* __itoa(int value, char* result, int base);

//
// WEB ADMIN
//

void phi_webadmin(int port, const char* wwwRoot);
char* phi_processJson(char *pJsonReq);
void phi_freeJsonReply(char* pJsonReply);

//
// LOGGING
//

int phi_logInit(char *filename);
void phi_logMsg(const char* tag, const char *fmt, ...);
void phi_logClose(void);

// Phi Link
BOOL startPhiLink(char* ipAddr, int port);


