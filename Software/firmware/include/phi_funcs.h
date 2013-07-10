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

UINT64 phi_upTime();

void phi_abort_process(int rc);
void phi_abort_with_msg(const char* msg);

//
// non-ANSI
//

char* __itoa(int value, char* result, int base);

//
// WEB ADMIN
//

void phi_webadmin(int port, const char* wwwRoot);

//
// LOGGING
//

int phi_log_start(char *filename);
void phi_log_msg(const char* tag, const char *fmt, ...);
void phi_log_close(void);



