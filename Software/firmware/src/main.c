
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
