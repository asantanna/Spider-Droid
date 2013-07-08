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

useconds_t phi_getMicros();
useconds_t phi_upTime();

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



