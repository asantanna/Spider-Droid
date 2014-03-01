
//
// Interface to UART (used for motors)
//

#include "phi.h"

// UART 0 file descriptor
static int uartFile = -1;

// At bootup, RPI pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively

BOOL uart_init() {

  // open UART 0 in write-only mode
  
  uartFile = open(UART_DRIVER_NAME, O_WRONLY);		

  if (uartFile < 0) {
    LOG_FATAL("uart_init: can't open UART device driver");
    return FALSE;
  }

  // set non-blocking mode
  // Note: this is done so that when the UART driver buffer fills, writes do not block
  // and instead return EAGAIN.

  if (setNonBlocking(uartFile) < 0) {
    LOG_FATAL("uart_init: can't set UART fd to non-blocking");
    return FALSE;
  }

  //
  // configure UART 0 using termios structure (see man termios)
  //
  
  //  The termios structure
  //    Many  of the functions described here have a termios_p argument that is
  //    a pointer to a termios structure.  This structure contains at least the
  //    following members:
  //
  //    tcflag_t c_iflag;      /* input modes */
  //    tcflag_t c_oflag;      /* output modes */
  //    tcflag_t c_cflag;      /* control modes */
  //    tcflag_t c_lflag;      /* local modes */
  //     cc_t    c_cc[NCCS];   /* special characters */
  
  // Some useful flags
  //
  // (c_cflag)
  //
  //	Baud rate -  B1200, B2400, B4800, B9600, B19200, B38400, B57600,
  //               B115200, B230400, B460800, B500000, B576000, B921600, B1000000,
  //               B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
  //
  //  CSIZE     Character size mask.  Values are CS5, CS6, CS7, or CS8.
  //  CLOCAL    Ignore modem control lines.
  //	CREAD     Enable receiver
  //  PARENB    Enable parity generation on output and parity checking for input.
  //  PARODD    If set, then parity for input and output is odd; otherwise even parity is used.
  //
  //  (c_iflag)
  //
  //  IGNPAR    Ignore framing errors and parity errors.
  //  IGNCR     Ignore carriage return on input.
  //  ICRNL     Translate  carriage  return to newline on input (unless IGNCR is set).
  //
  //  (c_oflag)
  //
  //  ONLCR     (XSI) Map NL to CR-NL on output.
  //  OCRNL     Map CR to NL on output.
  //  ONOCR     Don't output CR at column 0.
  //  ONLRET    Don't output CR.
  //

  struct termios options;
  tcgetattr(uartFile, &options);

  options.c_cflag = MC_DEF_BAUD | CS8 | CLOCAL;           // set baud rate, ignore modem lines, disable RX disabled (no CREAD)
  options.c_iflag = IGNPAR;                               // ignore parity errors (btw RX is disabled already above)
  options.c_oflag = 0;
  options.c_lflag = 0;

  //  tcsetattr() sets the parameters associated with  the  terminal  (unless
  //    support is required from the underlying hardware that is not available)
  //    from the termios structure referred to by termios_p.   optional_actions
  //    specifies when the changes take effect:
  //
  //    TCSANOW
  //    the change occurs immediately.
  //
  //    TCSADRAIN
  //    the change occurs after all output written to fd has been trans-
  //    mitted.  This function should be used when  changing  parameters
  //    that affect output.
  //
  //    TCSAFLUSH
  //    the  change  occurs  after  all  output  written  to  the object
  //    referred by fd has been transmitted, and all input that has been
  //    received  but  not  read  will be discarded before the change is
  //    made.
  //  

  uart_discardAll();
  tcsetattr(uartFile, TCSANOW, &options);

  return TRUE;
}

// functions for synchronizing access to UART
// Note: primitive calls are NOT synchronized, the caller is
// responsible for synchronization


// void uart_lock() {
//   PHI_MUTEX_GET(&mtxUART);
// }

// void uart_unlock() {
//   PHI_MUTEX_RELEASE(&mtxUART);
// }

// primitive UART access functions
// NOTE: these are NOT synchronized, caller must use uart_lock()/uart_unlock() if necessary

void uart_send(BYTE* pData, int dataLen) {
  
  if (uartFile >= 0)
  {
    int count = write(uartFile, pData, dataLen);
    if (count < 0)
    {
      perror("UART TX error");
      PHI_abortProcess(-1);
    }
  }
}

// this function is not used in Phi because we never read motor
// controller status because the controller TX lines can't be
// wire-ANDed together. (Code also never tested ...)

int uart_receive(void* pBuff, int buffLen) {

  // Note: Since we use non-blocking I/O, this function will exit if there are
  // no receive bytes waiting (non blocking read). If we want to hold
  // waiting for input, use a while loop when calling this or change
  // flags.

  int numRead = 0;

  if (uartFile >= 0)
  {
    numRead = read(uartFile, pBuff, buffLen);
    
    if (numRead < 0)
    {
      // an error occured (will also occur if there are no bytes says
      // sample ... hmmm)
      //
      // Note: maybe these two tests are only meaningful when
      // in blocking mode?
    }
    else if (numRead == 0)
    {
      // no data waiting
    }
  }

  return numRead;
}

void uart_flush() {
  //
  // tcdrain()  waits until all output written to the object referred to by fd has been transmitted.
  //
  tcdrain(uartFile);
}

// tcflush() discards data written to the object referred to by fd but not
//     transmitted,  or  data received but not read, depending on the value of
//     queue_selector:
//
//     TCIFLUSH
//            flushes data received but not read.
//
//     TCOFLUSH
//            flushes data written but not transmitted.
//
//     TCIOFLUSH
//            flushes both data received but not read, and  data  written  but
//            not transmitted.

void uart_discardInput() {
  tcflush(uartFile, TCIFLUSH);
}

void uart_discardOutput() {
  tcflush(uartFile, TCOFLUSH);
}

void uart_discardAll() {
  tcflush(uartFile, TCIOFLUSH);
}
  