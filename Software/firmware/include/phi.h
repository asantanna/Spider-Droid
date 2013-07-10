
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
#include <sys/time.h>
#include <netdb.h>      // for gethostbyname(), etc
#include <ifaddrs.h>    // for getifaddrs()
#include <unistd.h>			// for UART
#include <fcntl.h>			// for UART
#include <termios.h>		// for UART

// useful types
#include "phi_defs.h"

// polulu 2s9v1 motor controller
#include "polulu_2s9v1.h"

// abstract motor (hides controller details)
#include "motor.h"

// app globals
#include "globals.h"

// misc func decls
#include "phi_funcs.h"

