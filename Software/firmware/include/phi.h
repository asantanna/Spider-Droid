
//
// Main include file for PHI
//

// make sure we get thread-safe functions from libc
#define _REENTRANT 1

#define _GNU_SOURCE         // for strcasestr()

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
#include <sched.h>
#include <math.h>           // for pow()

#include <sys/ioctl.h>      // for driver access (SPI, etc)
#include <sys/time.h>
#include <sys/utsname.h>    // for uname()
#include <sys/sysinfo.h>    // for sysinfo()
#include <netinet/tcp.h>    // for TCP_NODELAY
#include <arpa/inet.h>      // for inet_addr

// BCM2835 GPIO
#include "bcm2835_gpio.h"

// useful types
#include "phi_defs.h"

// polulu 2s9v1 motor controller
#include "polulu_2s9v1.h"

// Parallax L3G4200D gyroscope module
#include "plax_L3G4200D.h"

// Microchip ADC chip
#include "mcp3008.h"

// abstract motor (hides controller details)
#include "motor.h"

// phi link defs
#include "philink.h"

// hardware abstraction layer (HAL)
#include "hal.h"
#include "hal_stub.h"
#include "hal_phi.h"

// datalog types
#include "datalog.h"

// event gates
#include "event_gate.h"

// misc func decls
#include "phi_funcs.h"

// app globals (THIS SHOULD BE LAST)
#include "globals.h"
