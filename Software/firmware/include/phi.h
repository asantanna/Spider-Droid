
//
// Main include file for spiderBot
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>			// used for UART
#include <fcntl.h>			// used for UART
#include <termios.h>		// used for UART

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

