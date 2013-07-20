
//
// spiderBot entry point
//

// create globals here
#define GLOBAL_INIT

#include "phi.h"

void main() {

  // remember start-up time
  struct timeval tv;
  gettimeofday(&tv, NULL);
  g_startupTime = TV_TO_USEC(tv);
  
  // startup message (in case log will fail)
  time_t now = time(NULL);
  char* sTime = ctime(&now);
  printf("PHI started at %s", sTime);
  
  // get IP of (probable) interface we will get requests from
  g_ipAddr = phi_getHostIP();
  
  // init log
  if (phi_logInit(LOGFILE_NAME) == 0) {
    // failed
    printf("Error: could not open log file '%s' (errno=%d)!  (Are you running as root?)\n", LOGFILE_NAME, errno);
    phi_abortProcess(-1);
  }

  // remove NL and log
  sTime[strlen(sTime)-1] = 0;
  LOG_INFO("####### PHI startup at %s #######", sTime);

  // start web admin server (never returns)
  phi_webadmin(80, "./wwwRoot");

  // never gets here
}

// initialize all PHI-specific hardware
//
// Note: all initialization that requires PHI hardware
// goes here.

char* phi_initPeripherals() {

  // NULL means success
  char *rc = NULL;

  g_initPeriph = FALSE;
  
  // set up UART for communication with motor controllers
  if (!uartInit()) {
    rc = "phi_InitPeripherals: Can't open UART. Are you running on PHI?";
    goto quick_exit;
  }

  // success
  g_initPeriph = TRUE;

quick_exit:
  
  return rc;
}
