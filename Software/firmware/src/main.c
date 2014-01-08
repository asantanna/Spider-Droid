
//
// spiderBot entry point
//

// create globals here
#define GLOBAL_INIT

#include "phi.h"

// uncomment to debug mem allocations
//
// You must also do:
//      "export MALLOC_TRACE=mtrace.out" in shell
//      cd bin
//      sudo ../bin-debug/phi-ubuntu
//
// #include <mcheck.h>
// #define PHI_DEBUG_MALLOC    1

// internal
void globalInit();

void main() {

  // initialize
  globalInit();

  // start the hardware pump threads
  startHwPump();

  //
  // Start web admin server
  //
  // Note: the main thread is dedicated to the admin web server
  // main loop.
  //
  
  phi_webadmin(80, "./wwwRoot");

  // never gets here
  LOG_FATAL("phi_webadmin returned!");
  
}

//
// Global intialization
//

void globalInit() {

#ifdef PHI_DEBUG_MALLOC
  mtrace();
#endif

  // remember start-up time
  struct timeval tv;
  gettimeofday(&tv, NULL);
  g_startupTime = TV_TO_USEC(tv);

  // startup message (in case log will fail)
  time_t now = time(NULL);
  char* sTime = ctime(&now);
  printf("PHI started at %s", sTime);

  // get system info
  if (uname(&g_uname) < 0) {
    LOG_FATAL("could not get system info - uname() failed");
  }

  // get IP of (probable) interface we will get requests from
  g_ipAddr = getHostIP();

  // init log
  if (phi_logInit(LOGFILE_NAME) == 0) {
    // failed
    LOG_FATAL("Error: could not open log file '%s' (errno=%d)!  (Are you running as root?)\n", LOGFILE_NAME, errno);
  }

  // remove NL and log
  sTime[strlen(sTime)-1] = 0;
  LOG_INFO("####### PHI startup at %s #######", sTime);

  // initialize HAL
  HAL_init();

  // init peripherals
  char* p = HAL_initPeripherals();

  if (p != NULL) {
    LOG_FATAL("Peripherals did not initialize.\n%s", p);
  } else {
    LOG_INFO("Peripherals initialized");
  }
}
