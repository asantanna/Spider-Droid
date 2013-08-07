
//
// Main include file for spiderBot
//

// make sure we get thread-safe functions from libc
#define _REENTRANT 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <netdb.h>          // for gethostbyname(), etc
#include <ifaddrs.h>        // for getifaddrs()
#include <unistd.h>         // for UART
#include <fcntl.h>          // for UART
#include <termios.h>        // for UART
#include <pthread.h>    
#include <sys/ioctl.h>      // for driver access (SPI, etc)
#include <sys/time.h>
#include <sys/utsname.h>    // for uname()
#include <sys/sysinfo.h>    // for sysinfo()

// useful types
#include "phi_defs.h"

// polulu 2s9v1 motor controller
#include "polulu_2s9v1.h"

// parallax L3G4200D gyroscope module
#include "plax_L3G4200D.h"

// abstract motor (hides controller details)
#include "motor.h"

// phi link defs
#include "philink.h"

// HAL
#include "hal.h"

// misc func decls
#include "phi_funcs.h"

// app globals (THIS SHOULD BE LAST)
#include "globals.h"
