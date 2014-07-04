
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

void main(int argc, char** argv) {

  // initialize
  globalInit();

  // process command line switches
  cmdline(argc, argv);

  // start the hardware pump threads now that we survived
  // the cmdline processing.  The cmdline processing can exit
  // on errors and when being used to create a test suite script.
  startHwPump();


  //
  // Start web admin server
  //
  // Note: the main thread is dedicated to the admin web server
  // main loop.
  //
  
  webAdmin(80, "./wwwRoot");

  // never gets here
  LOG_FATAL("webAdmin returned!");
  
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
  if (logInit(LOGFILE_NAME) == 0) {
    // failed
    LOG_FATAL("Error: could not open log file '%s' (errno=%d)!  (Are you running as root?)\n", LOGFILE_NAME, errno);
  }

  // remove NL and log
  sTime[strlen(sTime)-1] = 0;
  
  LOG_INFO("");
  LOG_INFO("#######");
  LOG_INFO("####### PHI startup at %s #######", sTime);
  LOG_INFO("#######");
  LOG_INFO("");

  // initialize HAL
  HAL_init();

  //
  // Init performance logs
  //

  // phiLink
  g_pDlog_phiLink_period        = dlog_create("PhiLink Period", "uS", PERFLOG_PHILINK_NUM_ELEM, PERFLOG_PHILINK_EPOCH);  

  // pump period
  g_pDlog_hwPump_UART_period    = dlog_create("UART Period", "uS", PERFLOG_HWPUMP_NUM_ELEM, PERFLOG_HWPUMP_EPOCH);  
  g_pDlog_hwPump_SPI_period     = dlog_create("SPI Period",  "uS", PERFLOG_HWPUMP_NUM_ELEM, PERFLOG_HWPUMP_EPOCH);
  g_pDlog_hwPump_I2C_period     = dlog_create("I2C Period",  "uS", PERFLOG_HWPUMP_NUM_ELEM, PERFLOG_HWPUMP_EPOCH);

  // pump work duration
  g_pDlog_hwPump_UART_workTime  = dlog_create("UART Work Dur", "uS", PERFLOG_HWPUMP_NUM_ELEM, PERFLOG_HWPUMP_EPOCH);
  g_pDlog_hwPump_SPI_workTime   = dlog_create("SPI Work Dur",  "uS", PERFLOG_HWPUMP_NUM_ELEM, PERFLOG_HWPUMP_EPOCH);
  g_pDlog_hwPump_I2C_workTime   = dlog_create("I2C Work Dur",  "uS", PERFLOG_HWPUMP_NUM_ELEM, PERFLOG_HWPUMP_EPOCH);

  //
  // Init peripherals
  //
  
  char* p = HAL_initPeripherals();

  if (p != NULL) {
    LOG_FATAL("Peripherals did not initialize.\n%s", p);
  } else {
    LOG_INFO("Peripherals initialized");
  }
}

//
// Shutdown
//

void globalShutdown() {

  TODO("globalShutdown() not fully implemented.");
  LOG_INFO("*** Shutting down hardware ***");

  // stop motors
  stopAllMotors();
}

