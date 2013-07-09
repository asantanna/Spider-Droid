
//
// spiderBot entry point
//

// create globals here
#define GLOBAL_INIT

#include "phi.h"

void main() {

  // remember start-up time
  gettimeofday(&g_startupTime, NULL);

  // startup message
  printf("PHI started at %ld.%ld\n", g_startupTime.tv_sec, g_startupTime.tv_usec);

  // init some stuff (TODO)

  // start web admin server (never returns)
  phi_webadmin(80, "./wwwRoot");

  // never gets here
}
